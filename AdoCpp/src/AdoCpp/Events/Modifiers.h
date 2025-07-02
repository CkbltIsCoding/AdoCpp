#pragma once
#include "Base.h"

namespace AdoCpp::Event::Modifiers
{
    class RepeatEvents final : public Event
    {
    public:
        enum class RepeatType
        {
            Beat,
            Floor
        };
        RepeatEvents() = default;
        explicit RepeatEvents(const rapidjson::Value& data);
        bool stackable() override { return true; }
        std::string name() override { return "RepeatEvents"; }
        RepeatEvents* clone() override { return new RepeatEvents(*this); }
        RepeatType repeatType = RepeatType::Beat;
        size_t repetitions = 1;
        size_t floorCount = 1;
        double interval = 1;
        bool executeOnCurrentFloor = false;
        std::vector<std::string> tag;
        double duration = 1;
    };
} // namespace AdoCpp::Event::Modifiers
