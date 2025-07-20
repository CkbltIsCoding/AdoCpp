#include "Dlc.h"

namespace AdoCpp::Event::Dlc
{
    Hold::Hold(const rapidjson::Value& data) : StaticEvent(data)
    {
        duration = data["duration"].GetDouble();
        distanceMultiplier = data["distanceMultiplier"].GetDouble();
        landingAnimation = data["landingAnimation"].GetBool();
    }
    rapidjson::Value Hold::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);
        autoRemoveDecimalPart(val, "duration", duration, alloc);
        autoRemoveDecimalPart(val, "distanceMultiplier", distanceMultiplier, alloc);
        val.AddMember("landingAnimation", landingAnimation, alloc);
        return val;
    }
} // namespace AdoCpp::Event::Dlc
