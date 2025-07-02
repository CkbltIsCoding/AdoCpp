#pragma once
#include <stdexcept>
#include <string>

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
    constexpr const char* const trackColorTypeCstr[] = {
        "Single", "Stripes", "Glow", "Blink", "Switch", "Rainbow", "Volume",
    };
    constexpr const char* trackColorType2cstr(const TrackColorType& trackColorType)
    {
        return trackColorTypeCstr[static_cast<int>(trackColorType)];
    }
    constexpr TrackColorType cstr2trackColorType(const char* cstr)
    {
        for (int i = 0; i < std::size(trackColorTypeCstr); ++i)
            if (strcmp(cstr, trackColorTypeCstr[i]) == 0)
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
    constexpr const char* const trackStyleCstr[] = {"Standard", "Neon", "NeonLight", "Basic", "Minimal", "Gems"};
    constexpr const char* trackStyle2cstr(const TrackStyle& trackStyle)
    {
        return trackStyleCstr[static_cast<int>(trackStyle)];
    }
    constexpr TrackStyle cstr2trackStyle(const char* cstr)
    {
        for (int i = 0; i < std::size(trackStyleCstr); ++i)
            if (strcmp(cstr, trackStyleCstr[i]) == 0)
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
    constexpr const char* const trackAnimationCstr[] = {
        // TODO
        "None", "Fade", "Scatter", "Scatter_Far", "Assemble", "Extend", "Grow_Spin"};
    constexpr const char* trackAnimation2cstr(const TrackAnimation& trackAnimation)
    {
        return trackAnimationCstr[static_cast<int>(trackAnimation)];
    }
    constexpr TrackAnimation cstr2trackAnimation(const char* cstr)
    {
        for (int i = 0; i < std::size(trackAnimationCstr); ++i)
            if (strcmp(cstr, trackAnimationCstr[i]) == 0)
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
    constexpr const char* const trackDisappearAnimationCstr[] = {
        // TODO
        "None", "Fade", "Scatter", "Scatter_Far", "Assemble", "Retract", "Shrink_Spin"};
    constexpr const char* trackDisappearAnimation2cstr(const TrackDisappearAnimation& trackDisappearAnimation)
    {
        return trackDisappearAnimationCstr[static_cast<int>(trackDisappearAnimation)];
    }
    constexpr TrackDisappearAnimation cstr2trackDisappearAnimation(const char* cstr)
    {
        for (int i = 0; i < std::size(trackDisappearAnimationCstr); ++i)
            if (strcmp(cstr, trackDisappearAnimationCstr[i]) == 0)
                return static_cast<TrackDisappearAnimation>(i);
        throw std::invalid_argument(cstr);
    }

    enum class TrackColorPulse
    {
        Backward = -1,
        None = 0,
        Forward = 1,
    };
    constexpr const char* const trackColorPulseCstr[] = {"Backward", "None", "Forward"};
    constexpr const char* trackColorPulse2cstr(const TrackColorPulse& trackColorPulse)
    {
        return trackColorPulseCstr[static_cast<int>(trackColorPulse) + 1];
    }
    constexpr TrackColorPulse cstr2trackColorPulse(const char* cstr)
    {
        for (int i = 0; i < std::size(trackColorPulseCstr); ++i)
            if (strcmp(cstr, trackColorPulseCstr[i]) == 0)
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
    };
} // namespace AdoCpp
