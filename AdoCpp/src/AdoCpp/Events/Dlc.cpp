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

        if (static_cast<int>(duration) == duration)
            val.AddMember("duration", static_cast<int>(duration), alloc);
        else
            val.AddMember("duration", duration, alloc);
        if (static_cast<int>(distanceMultiplier) == distanceMultiplier)
            val.AddMember("distanceMultiplier", static_cast<int>(distanceMultiplier), alloc);
        else
            val.AddMember("distanceMultiplier", distanceMultiplier, alloc);
        val.AddMember("landingAnimation", landingAnimation, alloc);
        return val;
    }
} // namespace AdoCpp::Event::Dlc
