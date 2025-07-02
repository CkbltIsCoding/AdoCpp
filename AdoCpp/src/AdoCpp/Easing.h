#pragma once
#include <map>
#include <stdexcept>
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

    constexpr const char* const easingCstr[34] = {
        "Linear",
        "InSine",    "OutSine",    "InOutSine",
        "InQuad",    "OutQuad",    "InOutQuad",
        "InCubic",   "OutCubic",   "InOutCubic",
        "InQuart",   "OutQuart",   "InOutQuart",
        "InQuint",   "OutQuint",   "InOutQuint",
        "InExpo",    "OutExpo",    "InOutExpo",
        "InCirc",    "OutCirc",    "InOutCirc",
        "InBack",    "OutBack",    "InOutBack",
        "InElastic", "OutElastic", "InOutElastic",
        "InBounce",  "OutBounce",  "InOutBounce",
        "InFlash",   "OutFlash",   "InOutFlash",
    };

    constexpr const char* easing2cstr(const Easing& easing)
    {
        return easingCstr[static_cast<int>(easing)];
    }

    constexpr Easing cstr2easing(const char* easing)
    {
        for (int i = 0; i < std::size(easingCstr); ++i)
            if (strcmp(easing, easingCstr[i]) == 0)
                return static_cast<Easing>(i);
        throw std::invalid_argument(easing);
    }

    double ease(Easing easing, double x);
}