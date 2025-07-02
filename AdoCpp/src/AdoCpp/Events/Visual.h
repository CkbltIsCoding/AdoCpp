#pragma once
#include "AdoCpp/Easing.h"
#include "AdoCpp/Utils.h"
#include "Base.h"

namespace AdoCpp::Event::Visual
{
    class MoveCamera final : public DynamicEvent
    {
    public:
        MoveCamera() = default;
        explicit MoveCamera(const rapidjson::Value& data);
        bool stackable() override { return true; }
        std::string name() override { return "MoveCamera"; }
        MoveCamera* clone() override { return new MoveCamera(*this); }
        double duration = 1;
        std::optional<RelativeToCamera> relativeTo;
        OptionalPoint position;
        std::optional<double> rotation;
        std::optional<double> zoom;
        Easing ease = Easing::Linear;
    };
} // namespace AdoCpp::Event::Visual
