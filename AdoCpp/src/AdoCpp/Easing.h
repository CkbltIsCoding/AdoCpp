#pragma once
#include <stdexcept>

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

    constexpr const char* const cstrEasing[] = {
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
        return cstrEasing[static_cast<int>(easing)];
    }

    constexpr Easing cstr2easing(const char* easing)
    {
        for (int i = 0; i < std::size(cstrEasing); ++i)
            if (strcmp(easing, cstrEasing[i]) == 0)
                return static_cast<Easing>(i);
        throw std::invalid_argument(easing);
    }

    [[nodiscard]] double ease(Easing easing, double x);
}