#include "GamePlay.h"
#include <cstring>
#include "rapidjson/document.h"

namespace AdoCpp::Event::GamePlay
{
    SetSpeed::SetSpeed(const rapidjson::Value& data) : DynamicEvent(data)
    {
        beatsPerMinute = data["beatsPerMinute"].GetDouble();
        if (!data.HasMember("speedType"))
        {
            speedType = SpeedType::Bpm;
            return;
        }
        if (strcmp(data["speedType"].GetString(), "Bpm") == 0)
            speedType = SpeedType::Bpm;
        else if (strcmp(data["speedType"].GetString(), "Multiplier") == 0)
            speedType = SpeedType::Multiplier;
        else
            throw std::invalid_argument("Invalid speedType");
        bpmMultiplier = data["bpmMultiplier"].GetDouble();
    }
    Twirl::Twirl(const rapidjson::Value& data) : StaticEvent(data) {}
    Pause::Pause(const rapidjson::Value& data) : StaticEvent(data)
    {
        duration = data["duration"].GetDouble();
        countdownTicks = data["countdownTicks"].GetDouble();
        if (data["angleCorrectionDir"].IsString())
        {
            if (const char* const str = data["angleCorrectionDir"].GetString(); strcmp(str, "Backward") == 0)
                angleCorrectionDir = AngleCorrectionDir::Backward;
            else if (strcmp(str, "None") == 0)
                angleCorrectionDir = AngleCorrectionDir::None;
            else if (strcmp(str, "Forward") == 0)
                angleCorrectionDir = AngleCorrectionDir::Forward;
            else
                throw std::invalid_argument("Invalid angleCorrectionDir");
        }
        else
        {
            if (const int dir = data["angleCorrectionDir"].GetInt(); -1 <= dir && dir <= 1)
                angleCorrectionDir = static_cast<AngleCorrectionDir>(data["angleCorrectionDir"].GetInt());
            else
                throw std::invalid_argument("Invalid angleCorrectionDir");
        }
    }

    SetPlanetRotation::SetPlanetRotation(const rapidjson::Value& data) : StaticEvent(data)
    {
        ease = cstr2easing(data["ease"].GetString());
        easeParts = data["easeParts"].GetUint64();
        easePartBehavior =
            !data.HasMember("easePartBehavior") || !strcmp(data["easePartBehavior"].GetString(), "Repeat")
            ? EasePartBehavior::Repeat
            : EasePartBehavior::Mirror;
    }
} // namespace AdoCpp::Event::GamePlay
