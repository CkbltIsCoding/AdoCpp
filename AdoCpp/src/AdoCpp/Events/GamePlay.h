#pragma once
#include <cstdint>
#include <string>

#include "AdoCpp/Easing.h"
#include "Base.h"
#include "rapidjson/document.h"

namespace AdoCpp::Event::GamePlay
{
    class SetSpeed final : public DynamicEvent
    {
    public:
        enum class SpeedType
        {
            Bpm,
            Multiplier
        };
        SetSpeed() = default;
        explicit SetSpeed(const rapidjson::Value& data);
        bool stackable() override { return true; }
        std::string name() override { return "SetSpeed"; }
        SetSpeed* clone() override { return new SetSpeed(*this); }
        SpeedType speedType = SpeedType::Bpm;
        double beatsPerMinute = 100;
        double bpmMultiplier = 1;
    };
    class Twirl final : public StaticEvent
    {
    public:
        Twirl() = default;
        explicit Twirl(const rapidjson::Value& data);
        bool stackable() override { return false; }
        std::string name() override { return "Twirl"; }
        Twirl* clone() override { return new Twirl(*this); }
    };
    class Pause final : public StaticEvent
    {
    public:
        Pause() = default;
        explicit Pause(const rapidjson::Value& data);
        bool stackable() override { return false; };
        std::string name() override { return "Pause"; };
        Pause* clone() override { return new Pause(*this); }
        double duration;
        double countdownTicks;
        enum class AngleCorrectionDir
        {
            Backward = -1,
            None = 0,
            Forward = 1
        };
        AngleCorrectionDir angleCorrectionDir;
    };
    class SetPlanetRotation final : public StaticEvent
    {
    public:
        enum class EasePartBehavior
        {
            Repeat,
            Mirror
        };
        SetPlanetRotation() = default;
        explicit SetPlanetRotation(const rapidjson::Value& data);
        bool stackable() override { return false; };
        std::string name() override { return "SetPlanetRotation"; };
        SetPlanetRotation* clone() override { return new SetPlanetRotation(*this); }
        Easing ease;
        uint64_t easeParts;
        EasePartBehavior easePartBehavior;
    };
} // namespace AdoCpp::Event::GamePlay
