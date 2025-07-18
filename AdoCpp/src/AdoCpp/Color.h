#pragma once
#include <cstdint>
#include <string>
namespace AdoCpp
{
    // Most of the code is copied from "SFML/Graphics/Color.hpp".
    class Color
    {
    public:
        constexpr Color() = default;

        constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha = 255);

        constexpr explicit Color(std::uint32_t color);
        constexpr explicit Color(const std::string& color);

        constexpr static Color fromHSV(double h, double s, double v);

        [[nodiscard]] constexpr std::tuple<double, double, double> toHSV() const;

        [[nodiscard]] constexpr std::uint32_t toInteger() const;

        enum class ToStringAlphaMode
        {
            Ignore,
            Auto,
            Show
        };

        [[nodiscard]] constexpr std::string toString(bool hash, bool uppercase, ToStringAlphaMode alphaMode) const;

        std::uint8_t r{};
        std::uint8_t g{};
        std::uint8_t b{};
        std::uint8_t a{255};
    };
} // namespace AdoCpp

#include "Color.inl"
