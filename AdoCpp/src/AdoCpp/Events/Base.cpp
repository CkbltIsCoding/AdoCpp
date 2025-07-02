#include "Base.h"

#include "AdoCpp/Utils.h"

namespace AdoCpp::Event
{
    Event::Event(const rapidjson::Value& data) { this->floor = data["floor"].GetUint64(); }
    StaticEvent::StaticEvent(const rapidjson::Value& data) : Event(data) {}
    DynamicEvent::DynamicEvent(const rapidjson::Value& data) : Event(data)
    {
        if (data.HasMember("angleOffset"))
            angleOffset = data["angleOffset"].GetDouble();
        if (data.HasMember("eventTag"))
            eventTag = cstr2tags(data["eventTag"].GetString());
    }
} // namespace AdoCpp::Event
