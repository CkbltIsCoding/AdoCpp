#include "Modifiers.h"
#include "AdoCpp/Utils.h"

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
} // namespace AdoCpp::Event::Modifiers
