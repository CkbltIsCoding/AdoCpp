#include "Easing.h"

#include <cmath>

namespace AdoCpp
{
    const double pi = 3.1415926,
        c1 = 1.70158,
        c2 = c1 * 1.525,
        c3 = c1 + 1,
        c4 = (2 * pi) / 3,
        c5 = (2 * pi) / 4.5,
        n1 = 7.5625,
        d1 = 2.75;
    
    static double out_bounce(double x)
    {
        if (x < 1 / d1) return n1 * x * x;
        if (x < 2 / d1)
        {
            x -= 1.5;
            return n1 * (x / d1) * x + 0.75;
        }
        if (x < 2.5 / d1)
        {
            x -= 2.25;
            return n1 * (x / d1) * x + 0.9375;
        }
        x -= 2.625;
        return n1 * (x / d1) * x + 0.984375;    
    }

    double ease(const Easing easing, const double x)
    {
        if (x <= 0)
            return 0;
        if (x >= 1)
            return 1;

        switch (easing)
        {
        case Easing::     Linear:  return x;
        case Easing::   InSine:    return 1 - cos((x * pi) / 2);
        case Easing::  OutSine:    return sin((x * pi) / 2);
        case Easing::InOutSine:    return -(cos(pi * x) - 1) / 2;
        case Easing   ::InQuad:    return x * x;
        case Easing::  OutQuad:    return 1 - (1 - x) * (1 - x);
        case Easing::InOutQuad:    return x < 0.5 ? 2 * x * x : 1 - (-2 * x + 2) * (-2 * x + 2) / 2;
        case Easing   ::InCubic:   return x * x * x;
        case Easing::  OutCubic:   return 1 - (1 - x) * (1 - x) * (1 - x);
        case Easing::InOutCubic:   return x < 0.5 ? 4 * x * x * x : 1 - (-2 * x + 2) * (-2 * x + 2) * (-2 * x + 2) / 2;
        case Easing   ::InQuart:   return x * x * x * x;
        case Easing::  OutQuart:   return 1 - (1 - x) * (1 - x) * (1 - x) * (1 - x);
        case Easing::InOutQuart:   return x < 0.5 ? 8 * x * x * x * x : 1 - (-2 * x + 2) * (-2 * x + 2) * (-2 * x + 2) * (-2 * x + 2) / 2;
        case Easing   ::InQuint:   return x * x * x * x * x;
        case Easing::  OutQuint:   return 1 - (1 - x) * (1 - x) * (1 - x) * (1 - x) * (1 - x);
        case Easing::InOutQuint:   return x < 0.5 ? 16 * x * x * x * x * x : 1 - (-2 * x + 2) * (-2 * x + 2) * (-2 * x + 2) * (-2 * x + 2) * (-2 * x + 2) / 2;
        case Easing   ::InExpo:    return pow(2, 10 * x - 10);
        case Easing::  OutExpo:    return 1 - pow(2, -10 * x);
        case Easing::InOutExpo:    return x < 0.5 ? pow(2, 20 * x - 10) / 2 : (2 - pow(2, -20 * x + 10)) / 2;
        case Easing   ::InCirc:    return 1 - sqrt(1 - x * x);
        case Easing::  OutCirc:    return sqrt(1 - (x - 1) * (x - 1));
        case Easing::InOutCirc:    return x < 0.5 ? (1 - sqrt(1 - pow(2 * x, 2))) / 2 : (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
        case Easing   ::InBack:    return c3 * x * x * x - c1 * x * x;
        case Easing::  OutBack:    return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
        case Easing::InOutBack:    return x < 0.5 ? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2 : (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
        case Easing   ::InElastic: return -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
        case Easing::  OutElastic: return pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
        case Easing::InOutElastic: return x < 0.5 ? -(pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5)) / 2 : (pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5)) / 2 + 1;
        case Easing   ::InBounce:  return 1 - out_bounce(1 - x);
        case Easing::  OutBounce:  return out_bounce(x);
        case Easing::InOutBounce:  return x < 0.5 ? (1 - out_bounce(1 - 2 * x)) / 2 : (1 + out_bounce(2 * x - 1)) / 2;
        default:
            break;
        }
        return 0;
    }

}
