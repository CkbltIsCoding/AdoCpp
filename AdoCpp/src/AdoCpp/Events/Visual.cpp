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
} // namespace AdoCpp::Event::Visual
