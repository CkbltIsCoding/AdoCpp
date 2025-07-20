#include "Base.h"

namespace AdoCpp::Event
{
    Event::Event(const rapidjson::Value& data)
    {
        this->floor = data["floor"].GetUint64();
        this->active = !data.HasMember("active") || toBool(data["active"]);
    }
    rapidjson::Document Event::intoJson() const
    {
        rapidjson::Document doc;
        doc.CopyFrom(intoJson(doc.GetAllocator()), doc.GetAllocator());
        return doc;
    }
    StaticEvent::StaticEvent(const rapidjson::Value& data) : Event(data) {}
    DynamicEvent::DynamicEvent(const rapidjson::Value& data) : Event(data)
    {
        if (data.HasMember("angleOffset"))
            angleOffset = data["angleOffset"].GetDouble();
        if (data.HasMember("eventTag"))
            eventTag = cstr2tags(data["eventTag"].GetString());
    }
} // namespace AdoCpp::Event
