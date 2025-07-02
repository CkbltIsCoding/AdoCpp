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

        [[nodiscard]] constexpr std::uint32_t toInteger() const;
        [[nodiscard]] constexpr std::string toString(bool hash, bool uppercase, bool ignoreAlpha) const;

        std::uint8_t r{};
        std::uint8_t g{};
        std::uint8_t b{};
        std::uint8_t a{255};
    };
} // namespace AdoCpp

#include "Color.inl"
