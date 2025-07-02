////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2025 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Angle.h"

#include <cassert>


namespace AdoCpp
{
    namespace priv
    {
        inline constexpr double pi  = 3.141592654f;
        inline constexpr double tau = pi * 2.0;

        constexpr double positiveRemainder(double a, double b)
        {
            assert(b > 0.0 && "Cannot calculate remainder with non-positive divisor");
            const double val = a - static_cast<double>(static_cast<int>(a / b)) * b;
            return val >= 0.0 ? val : val + b;
        }
    }

    ////////////////////////////////////////////////////////////
    constexpr double Angle::deg() const
    {
        return m_degrees;
    }


    ////////////////////////////////////////////////////////////
    constexpr double Angle::rad() const
    {
        return m_degrees / 180.0 * priv::pi;
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle Angle::wrapSigned() const
    {
        return degrees(priv::positiveRemainder(m_degrees + 180.0, 360.0) - 180.0);
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle Angle::wrapUnsigned() const
    {
        return radians(priv::positiveRemainder(m_degrees, 360.0));
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle::Angle(double degrees) : m_degrees(degrees)
    {
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle degrees(double angle)
    {
        return Angle(angle);
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle radians(double angle)
    {
        return Angle(angle * 180.0 / priv::pi);
    }


    ////////////////////////////////////////////////////////////
    constexpr bool operator==(Angle left, Angle right)
    {
        return left.deg() == right.deg();
    }


    ////////////////////////////////////////////////////////////
    constexpr bool operator!=(Angle left, Angle right)
    {
        return left.deg() != right.deg();
    }


    ////////////////////////////////////////////////////////////
    constexpr bool operator<(Angle left, Angle right)
    {
        return left.deg() < right.deg();
    }


    ////////////////////////////////////////////////////////////
    constexpr bool operator>(Angle left, Angle right)
    {
        return left.deg() > right.deg();
    }


    ////////////////////////////////////////////////////////////
    constexpr bool operator<=(Angle left, Angle right)
    {
        return left.deg() <= right.deg();
    }


    ////////////////////////////////////////////////////////////
    constexpr bool operator>=(Angle left, Angle right)
    {
        return left.deg() >= right.deg();
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator-(Angle right)
    {
        return degrees(-right.deg());
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator+(Angle left, Angle right)
    {
        return degrees(left.deg() + right.deg());
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle& operator+=(Angle& left, Angle right)
    {
        return left = left + right;
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator-(Angle left, Angle right)
    {
        return degrees(left.deg() - right.deg());
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle& operator-=(Angle& left, Angle right)
    {
        return left = left - right;
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator*(Angle left, double right)
    {
        return degrees(left.deg() * right);
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator*(double left, Angle right)
    {
        return right * left;
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle& operator*=(Angle& left, double right)
    {
        return left = left * right;
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator/(Angle left, double right)
    {
        assert(right != 0.0 && "Angle::operator/ cannot divide by 0");
        return degrees(left.deg() / right);
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle& operator/=(Angle& left, double right)
    {
        assert(right != 0.0 && "Angle::operator/= cannot divide by 0");
        return left = left / right;
    }


    ////////////////////////////////////////////////////////////
    constexpr double operator/(Angle left, Angle right)
    {
        assert(right.asDegrees() != 0.0 && "Angle::operator/ cannot divide by 0");
        return left.deg() / right.deg();
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator%(Angle left, Angle right)
    {
        assert(right.asDegrees() != 0.0 && "Angle::operator% cannot modulus by 0");
        return degrees(priv::positiveRemainder(left.deg(), right.deg()));
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle& operator%=(Angle& left, Angle right)
    {
        assert(right.asDegrees() != 0.0 && "Angle::operator%= cannot modulus by 0");
        return left = left % right;
    }

    namespace Literals
    {

    ////////////////////////////////////////////////////////////
    constexpr Angle operator""_deg(long double angle)
    {
        return degrees(static_cast<double>(angle));
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator""_deg(unsigned long long angle)
    {
        return degrees(static_cast<double>(angle));
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator""_rad(long double angle)
    {
        return radians(static_cast<double>(angle));
    }


    ////////////////////////////////////////////////////////////
    constexpr Angle operator""_rad(unsigned long long angle)
    {
        return radians(static_cast<double>(angle));
    }

    } // namespace Literals


    ////////////////////////////////////////////////////////////
    // Static member data
    ////////////////////////////////////////////////////////////

    // Note: the 'inline' keyword here is technically not required, but VS2019 fails
    // to compile with a bogus "multiple definition" error if not explicitly used.
    inline constexpr Angle Angle::Zero;

}
