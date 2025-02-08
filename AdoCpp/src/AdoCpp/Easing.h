#pragma once
#include <map>
#include <cmath>
#include <string>

namespace AdoCpp
{
    enum class Easing
    {
        Linear,
        InSine,    OutSine,    InOutSine,
        InQuad,    OutQuad,    InOutQuad,
        InCubic,   OutCubic,   InOutCubic,
        InQuart,   OutQuart,   InOutQuart,
        InQuint,   OutQuint,   InOutQuint,
        InExpo,    OutExpo,    InOutExpo,
        InCirc,    OutCirc,    InOutCirc,
        InBack,    OutBack,    InOutBack,
        InElastic, OutElastic, InOutElastic,
        InBounce,  OutBounce,  InOutBounce,
        InFlash,   OutFlash,   InOutFlash,
    };

    extern std::map<Easing, std::string> easing2string;
    extern std::map<std::string, Easing> string2easing;

    double ease(Easing easing, double x);
}