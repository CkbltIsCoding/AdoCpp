#pragma once

#include <optional>
#include <rapidjson/document.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "Color.h"
#include "Math/Vector2.h"

namespace AdoCpp
{
    [[noreturn]] inline void unreachable()
    {
        // Uses compiler specific extensions if possible.
        // Even if no extension is used, undefined behavior is still raised by
        // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
        __assume(false);
#else // GCC, Clang
        __builtin_unreachable();
#endif
    }

    typedef std::pair<std::optional<double>, std::optional<double>> OptionalPoint;

    enum RelativeToTile
    {
        Start,
        ThisTile,
        End
    };
    constexpr const char* const relativeToTileCstr[] = {"Start", "ThisTile", "End"};
    constexpr RelativeToTile cstr2relativeToTile(const char* const cstr)
    {
        for (int i = 0; i < std::size(relativeToTileCstr); ++i)
            if (strcmp(cstr, relativeToTileCstr[i]) == 0)
                return static_cast<RelativeToTile>(i);
        throw std::invalid_argument("Invalid Cstr");
    }
    constexpr const char* relativeToTile2cstr(const RelativeToTile& rel)
    {
        return relativeToTileCstr[static_cast<int>(rel)];
    }

    struct RelativeIndex
    {
        RelativeIndex() = default;
        RelativeIndex(const int64_t& index, const RelativeToTile& relativeTo) : index(index), relativeTo(relativeTo) {}

        explicit RelativeIndex(const rapidjson::Value& data) :
            index(data[0].GetInt64()), relativeTo(cstr2relativeToTile(data[1].GetString()))
        {
        }
        int64_t index{};
        RelativeToTile relativeTo{};
    };

    enum class RelativeToCamera
    {
        Player,
        Tile,
        Global,
        LastPosition
    };
    constexpr const char* const cstr_RelativeToCamera[] = {"Player", "Tile", "Global", "LastPosition"};
    constexpr RelativeToCamera cstr2relativeToCamera(const char* const cstr)
    {
        for (int i = 0; i < std::size(cstr_RelativeToCamera); ++i)
            if (strcmp(cstr, cstr_RelativeToCamera[i]) == 0)
                return static_cast<RelativeToCamera>(i);
        throw std::invalid_argument("Invalid Cstr");
    }
    constexpr const char* relativeToCamera2cstr(const RelativeToCamera& rel)
    {
        return cstr_RelativeToCamera[static_cast<int>(rel)];
    }

    /**
     * @brief Convert json data to bool.
     * @param data The json data.
     * @return The bool.
     * data == true || data == "Enabled" => true.
     * data == false || data == "Disabled" => false.
     * Otherwise, throw an exception.
     */
    bool toBool(const rapidjson::Value& data);

    /**
     * @brief Convert bpm to spb(seconds per beat).
     * @param bpm The bpm.
     * @return The spb.
     */
    inline double bpm2crotchet(const double bpm) { return 60.0 / bpm; }
    /**
     * @brief Convert bpm to mspb(milliseconds per beat).
     * @param bpm The bpm.
     * @return The mspb.
     */
    inline double bpm2mspb(const double bpm)
    {
        //return 60 / bpm * 1000;
        return 60000 / bpm;
    }
    /**
     * @brief Get the included angle.
     * @param angleDeg
     * @param nextAngleDeg
     * @return
     */
    double includedAngle(double angleDeg, double nextAngleDeg);

    std::vector<std::string> cstr2tags(const char* str);
} // namespace AdoCpp
