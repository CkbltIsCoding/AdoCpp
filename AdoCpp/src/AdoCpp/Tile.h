#pragma once
#include <stdexcept>

namespace AdoCpp
{
    enum Orbit
    {
        CounterClockwise,
        Clockwise,
    };
    inline Orbit operator!(const Orbit orbit) { return orbit == Clockwise ? CounterClockwise : Clockwise; }

    enum class TrackColorType
    {
        Single,
        Stripes,
        Glow,
        Blink,
        Switch,
        Rainbow,
        Volume,
    };
    constexpr const char* const cstrTrackColorType[] = {
        "Single", "Stripes", "Glow", "Blink", "Switch", "Rainbow", "Volume",
    };
    constexpr const char* trackColorType2cstr(const TrackColorType& trackColorType)
    {
        return cstrTrackColorType[static_cast<int>(trackColorType)];
    }
    constexpr TrackColorType cstr2trackColorType(const char* cstr)
    {
        for (int i = 0; i < std::size(cstrTrackColorType); ++i)
            if (strcmp(cstr, cstrTrackColorType[i]) == 0)
                return static_cast<TrackColorType>(i);
        throw std::invalid_argument(cstr);
    }

    enum class TrackStyle
    {
        Standard,
        Neon,
        NeonLight,
        Basic,
        Minimal,
        Gems,
    };
    constexpr const char* const cstrTrackStyle[] = {"Standard", "Neon", "NeonLight", "Basic", "Minimal", "Gems"};
    constexpr const char* trackStyle2cstr(const TrackStyle& trackStyle)
    {
        return cstrTrackStyle[static_cast<int>(trackStyle)];
    }
    constexpr TrackStyle cstr2trackStyle(const char* cstr)
    {
        for (int i = 0; i < std::size(cstrTrackStyle); ++i)
            if (strcmp(cstr, cstrTrackStyle[i]) == 0)
                return static_cast<TrackStyle>(i);
        throw std::invalid_argument(cstr);
    }

    enum class TrackAnimation
    {
        // TODO
        None,
        Fade,
        Scatter,
        Scatter_Far,
        Assemble,
        Extend,
        Grow_Spin
    };
    constexpr const char* const cstrTrackAnimation[] = {
        // TODO
        "None", "Fade", "Scatter", "Scatter_Far", "Assemble", "Extend", "Grow_Spin"};
    constexpr const char* trackAnimation2cstr(const TrackAnimation& trackAnimation)
    {
        return cstrTrackAnimation[static_cast<int>(trackAnimation)];
    }
    constexpr TrackAnimation cstr2trackAnimation(const char* cstr)
    {
        for (int i = 0; i < std::size(cstrTrackAnimation); ++i)
            if (strcmp(cstr, cstrTrackAnimation[i]) == 0)
                return static_cast<TrackAnimation>(i);
        throw std::invalid_argument(cstr);
    }
    enum class TrackDisappearAnimation
    {
        // TODO
        None,
        Fade,
        Scatter,
        Scatter_Far,
        Retract,
        Shrink_Spin,
    };
    constexpr const char* const cstrTrackDisappearAnimation[] = {
        // TODO
        "None", "Fade", "Scatter", "Scatter_Far", "Assemble", "Retract", "Shrink_Spin"};
    constexpr const char* trackDisappearAnimation2cstr(const TrackDisappearAnimation& trackDisappearAnimation)
    {
        return cstrTrackDisappearAnimation[static_cast<int>(trackDisappearAnimation)];
    }
    constexpr TrackDisappearAnimation cstr2trackDisappearAnimation(const char* cstr)
    {
        for (int i = 0; i < std::size(cstrTrackDisappearAnimation); ++i)
            if (strcmp(cstr, cstrTrackDisappearAnimation[i]) == 0)
                return static_cast<TrackDisappearAnimation>(i);
        throw std::invalid_argument(cstr);
    }

    enum class TrackColorPulse
    {
        Backward = -1,
        None = 0,
        Forward = 1,
    };
    constexpr const char* const cstrTrackColorPulse[] = {"Backward", "None", "Forward"};
    constexpr const char* trackColorPulse2cstr(const TrackColorPulse& trackColorPulse)
    {
        return cstrTrackColorPulse[static_cast<int>(trackColorPulse) + 1];
    }
    constexpr TrackColorPulse cstr2trackColorPulse(const char* cstr)
    {
        for (int i = 0; i < std::size(cstrTrackColorPulse); ++i)
            if (strcmp(cstr, cstrTrackColorPulse[i]) == 0)
                return static_cast<TrackColorPulse>(i - 1);
        throw std::invalid_argument(cstr);
    }

    enum class Hitsound
    {
        None,
        Kick,
        Sizzle,
        Shaker,
        FireTile,
        Hat,
        VehiclePositive,
        VehicleNegative,
        Squareshot,
        PowerDown,
        ReverbClap,
        ReverbClack,
        Hammer,
        SnareAcoustic2,
        SnareHouse,
        Sidestick,
        HatHouse,
        ShakerLoud,
        Chuck,
        KickHouse,
        KickRupture
    };
    constexpr const char* const cstrHitsound[] = {
        "None",
        "Kick",
        "Sizzle",
        "Shaker",
        "FireTile",
        "Hat",
        "VehiclePositive",
        "VehicleNegative",
        "Squareshot",
        "PowerDown",
        "ReverbClap",
        "ReverbClack",
        "Hammer",
        "SnareAcoustic2",
        "SnareHouse",
        "Sidestick",
        "HatHouse",
        "ShakerLoud",
        "Chuck",
        "KickHouse",
        "KickRupture"
    };
    constexpr const char* hitsound2cstr(const Hitsound& hitsound)
    {
        return cstrHitsound[static_cast<int>(hitsound)];
    }
    constexpr Hitsound cstr2hitsound(const char* cstr)
    {
        for (int i = 0; i < std::size(cstrHitsound); ++i)
            if (strcmp(cstr, cstrHitsound[i]) == 0)
                return static_cast<Hitsound>(i);
        throw std::invalid_argument(cstr);
    }
} // namespace AdoCpp
