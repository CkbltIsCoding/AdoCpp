#include "Visual.h"

namespace AdoCpp::Event::Visual
{
    MoveCamera::MoveCamera(const rapidjson::Value& data) : DynamicEvent(data)
    {
        duration = data["duration"].GetDouble();
        if (data.HasMember("relativeTo"))
            relativeTo = cstr2relativeToCamera(data["relativeTo"].GetString());
        if (data.HasMember("position"))
        {
            if (!data["position"][0].IsNull())
                position.first = data["position"][0].GetDouble();
            if (!data["position"][1].IsNull())
                position.second = data["position"][1].GetDouble();
        }
        if (data.HasMember("rotation"))
            rotation = data["rotation"].GetDouble();
        if (data.HasMember("zoom"))
            zoom = data["zoom"].GetDouble();
        ease = cstr2easing(data["ease"].GetString());
    }
    rapidjson::Value MoveCamera::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);
        if (!active)
            val.AddMember("active", active, alloc);
        if (static_cast<int>(duration) == duration)
            val.AddMember("duration", static_cast<int>(duration), alloc);
        else
            val.AddMember("duration", duration, alloc);
        if (relativeTo)
            val.AddMember("relativeTo", rapidjson::StringRef(relativeToCamera2cstr(*relativeTo)), alloc);
        // ReSharper disable once CppLocalVariableMayBeConst
        if (auto op = optionalPoint2json(position, alloc); op)
            val.AddMember("position", *op, alloc);
        if (rotation)
        {
            if (static_cast<int>(*rotation) == *rotation)
                val.AddMember("rotation", static_cast<int>(*rotation), alloc);
            else
                val.AddMember("rotation", *rotation, alloc);
        }
        if (zoom)
        {
            if (static_cast<int>(*zoom) == *zoom)
                val.AddMember("zoom", static_cast<int>(*zoom), alloc);
            else
                val.AddMember("zoom", *zoom, alloc);
        }
        val.AddMember("ease", rapidjson::StringRef(easing2cstr(ease)), alloc);
        if (static_cast<int>(angleOffset) == angleOffset)
            val.AddMember("angleOffset", static_cast<int>(angleOffset), alloc);
        else
            val.AddMember("angleOffset", angleOffset, alloc);
        char tagBuf[1145]{};
        tags2cstr(eventTag, tagBuf, 1145);
        rapidjson::Value tagValue;
        tagValue.SetString(tagBuf, strlen(tagBuf), alloc);
        val.AddMember("eventTag", tagValue, alloc);
        return val;
    }
} // namespace AdoCpp::Event::Visual
