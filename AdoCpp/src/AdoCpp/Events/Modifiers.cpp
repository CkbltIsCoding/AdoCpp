#include "Modifiers.h"

namespace AdoCpp::Event::Modifiers
{
    RepeatEvents::RepeatEvents(const rapidjson::Value& data) : Event(data)
    {
        if (data.HasMember("repeatType") && !strcmp(data["repeatType"].GetString(), "Floor"))
            repeatType = RepeatType::Floor;
        else
            repeatType = RepeatType::Beat;
        repetitions = data["repetitions"].GetUint64();
        floorCount = data.HasMember("floorCount") ? data["floorCount"].GetUint64() : 0;
        interval = data["interval"].GetDouble();
        executeOnCurrentFloor = data.HasMember("executeOnCurrentFloor") ? toBool(data["executeOnCurrentFloor"]) : false;
        tag = cstr2tags(data["tag"].GetString());
    }
    rapidjson::Value RepeatEvents::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);
        if (!active)
            val.AddMember("active", active, alloc);
        if (repeatType == RepeatType::Floor)
            val.AddMember("repeatType", "Floor", alloc);
        else if (repeatType == RepeatType::Beat)
            val.AddMember("repeatType", "Beat", alloc);
        val.AddMember("repetitions", repetitions, alloc);
        val.AddMember("floorCount", floorCount, alloc);
        if (static_cast<int>(interval) == interval)
            val.AddMember("interval", static_cast<int>(interval), alloc);
        else
            val.AddMember("interval", interval, alloc);
        val.AddMember("executeOnCurrentFloor", executeOnCurrentFloor, alloc);
        char tagBuf[1145]{};
        tags2cstr(tag, tagBuf, 1145);
        rapidjson::Value tagValue;
        tagValue.SetString(tagBuf, strlen(tagBuf), alloc);
        val.AddMember("tag", tagValue, alloc);
        return val;
    }
} // namespace AdoCpp::Event::Modifiers
