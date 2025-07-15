#pragma once
#include "Color.h"

namespace AdoCpp
{
    // Most of the code is copied from "SFML/Graphics/Color.inl".
    constexpr Color::Color(const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue,
                           const std::uint8_t alpha) : r(red), g(green), b(blue), a(alpha)
    {
    }

    constexpr Color::Color(const std::uint32_t color) :
        r(static_cast<std::uint8_t>((color & 0xff000000) >> 24)),
        g(static_cast<std::uint8_t>((color & 0x00ff0000) >> 16)),
        b(static_cast<std::uint8_t>((color & 0x0000ff00) >> 8)), a(static_cast<std::uint8_t>(color & 0x000000ff))
    {
    }

    constexpr Color::Color(const std::string& color)
    {
        const auto charToDigit16 = [](const char c)
        {
            return static_cast<std::uint8_t>('0' <= c && c <= '9'       ? c - '0'
                                                 : 'A' <= c && c <= 'F' ? c - 'A' + 10
                                                                        : c - 'a' + 10);
        };
        const size_t hash = color[0] == '#';
        r = charToDigit16(color[hash + 0]) * 16 + charToDigit16(color[hash + 1]);
        g = charToDigit16(color[hash + 2]) * 16 + charToDigit16(color[hash + 3]);
        b = charToDigit16(color[hash + 4]) * 16 + charToDigit16(color[hash + 5]);
        if (color.size() == hash + 8)
            a = charToDigit16(color[hash + 6]) * 16 + charToDigit16(color[hash + 7]);
    }

    constexpr std::uint32_t Color::toInteger() const
    {
        return static_cast<std::uint32_t>((r << 24) | (g << 16) | (b << 8) | a);
    }

    constexpr std::string Color::toString(const bool hash, const bool uppercase, const ToStringAlphaMode alphaMode) const
    {
        std::string str;
        if (hash)
            str.push_back('#');
        const auto digit16ToChar = [uppercase](const std::uint8_t v)
        {
            return static_cast<char>(v < 10 ? v + '0' : uppercase ? v - 10 + 'A' : v - 10 + 'a');
        };
        str.push_back(digit16ToChar(r / 16)), str.push_back(digit16ToChar(r % 16));
        str.push_back(digit16ToChar(g / 16)), str.push_back(digit16ToChar(g % 16));
        str.push_back(digit16ToChar(b / 16)), str.push_back(digit16ToChar(b % 16));

        using enum ToStringAlphaMode;
        if (alphaMode != Ignore && (alphaMode == Show || alphaMode == Auto && a != 255))
            str.push_back(digit16ToChar(a / 16)), str.push_back(digit16ToChar(a % 16));
        return str;
    }

    constexpr bool operator==(const Color left, const Color right)
    {
        return (left.r == right.r) && (left.g == right.g) && (left.b == right.b) && (left.a == right.a);
    }

    constexpr bool operator!=(const Color left, const Color right) { return !(left == right); }

    constexpr Color operator+(const Color left, const Color right)
    {
        const auto clampedAdd = [](std::uint8_t lhs, std::uint8_t rhs)
        {
            const int intResult = int{lhs} + int{rhs};
            return static_cast<std::uint8_t>(intResult < 255 ? intResult : 255);
        };

        return {clampedAdd(left.r, right.r), clampedAdd(left.g, right.g), clampedAdd(left.b, right.b),
                clampedAdd(left.a, right.a)};
    }

    constexpr Color operator-(const Color left, const Color right)
    {
        const auto clampedSub = [](std::uint8_t lhs, std::uint8_t rhs)
        {
            const int intResult = int{lhs} - int{rhs};
            return static_cast<std::uint8_t>(intResult > 0 ? intResult : 0);
        };

        return {clampedSub(left.r, right.r), clampedSub(left.g, right.g), clampedSub(left.b, right.b),
                clampedSub(left.a, right.a)};
    }

    constexpr Color operator*(const Color left, const Color right)
    {
        const auto scaledMul = [](std::uint8_t lhs, std::uint8_t rhs)
        {
            const auto uint16Result = static_cast<std::uint16_t>(std::uint16_t{lhs} * std::uint16_t{rhs});
            return static_cast<std::uint8_t>(uint16Result / 255u);
        };

        return {scaledMul(left.r, right.r), scaledMul(left.g, right.g), scaledMul(left.b, right.b),
                scaledMul(left.a, right.a)};
    }

    constexpr Color& operator+=(Color& left, const Color right) { return left = left + right; }

    constexpr Color& operator-=(Color& left, const Color right) { return left = left - right; }

    constexpr Color& operator*=(Color& left, const Color right) { return left = left * right; }
} // namespace AdoCpp
