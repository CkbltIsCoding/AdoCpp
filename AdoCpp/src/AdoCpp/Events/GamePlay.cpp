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
    rapidjson::Value SetSpeed::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);

        if (speedType == SpeedType::Bpm)
            val.AddMember("speedType", "Bpm", alloc);
        else
            val.AddMember("speedType", "Multiplier", alloc);
        if (static_cast<int>(beatsPerMinute) == beatsPerMinute)
            val.AddMember("beatsPerMinute", static_cast<int>(beatsPerMinute), alloc);
        else
            val.AddMember("beatsPerMinute", beatsPerMinute, alloc);
        if (static_cast<int>(bpmMultiplier) == bpmMultiplier)
            val.AddMember("bpmMultiplier", static_cast<int>(bpmMultiplier), alloc);
        else
            val.AddMember("bpmMultiplier", bpmMultiplier, alloc);
        if (static_cast<int>(angleOffset) == angleOffset)
            val.AddMember("angleOffset", static_cast<int>(angleOffset), alloc);
        else
            val.AddMember("angleOffset", angleOffset, alloc);
        return val;
    }
    Twirl::Twirl(const rapidjson::Value& data) : StaticEvent(data) {}

    rapidjson::Value Twirl::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);

        return val;
    }
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
    rapidjson::Value Pause::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);

        if (static_cast<int>(duration) == duration)
            val.AddMember("duration", static_cast<int>(duration), alloc);
        else
            val.AddMember("duration", duration, alloc);
        if (static_cast<int>(countdownTicks) == countdownTicks)
            val.AddMember("countdownTicks", static_cast<int>(countdownTicks), alloc);
        else
            val.AddMember("countdownTicks", countdownTicks, alloc);
        if (angleCorrectionDir == AngleCorrectionDir::None)
            val.AddMember("angleCorrectionDir", "None", alloc);
        else if (angleCorrectionDir == AngleCorrectionDir::Forward)
            val.AddMember("angleCorrectionDir", "Forward", alloc);
        else
            val.AddMember("angleCorrectionDir", "Backward", alloc);
        return val;
    }
    SetHitsound::SetHitsound(const rapidjson::Value& data)
    {
        if (!data.HasMember("gameSound") || !strcmp(data["gameSound"].GetString(), "Hitsound"))
            gameSound = GameSound::Hitsound;
        else if (!strcmp(data["gameSound"].GetString(), "Midspin"))
            gameSound = GameSound::Midspin;
        else
            throw std::invalid_argument("Invalid gameSound");
        hitsound = cstr2hitsound(data["hitsound"].GetString());
        hitsoundVolume = data["hitsoundVolume"].GetDouble();
    }
    rapidjson::Value SetHitsound::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);

        if (gameSound == GameSound::Hitsound)
            val.AddMember("gameSound", "Hitsound", alloc);
        else if (gameSound == GameSound::Midspin)
            val.AddMember("gameSound", "Midspin", alloc);
        val.AddMember("hitsound", rapidjson::StringRef(hitsound2cstr(hitsound)), alloc);
        if (static_cast<int>(hitsoundVolume) == hitsoundVolume)
            val.AddMember("hitsoundVolume", static_cast<int>(hitsoundVolume), alloc);
        else
            val.AddMember("hitsoundVolume", hitsoundVolume, alloc);
        return val;
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
    rapidjson::Value SetPlanetRotation::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);
        if (!active)
            val.AddMember("active", active, alloc);
        val.AddMember("ease", rapidjson::StringRef(easing2cstr(ease)), alloc);
        val.AddMember("easeParts", easeParts, alloc);
        if (easePartBehavior == EasePartBehavior::Repeat)
            val.AddMember("easePartBehavior", "Repeat", alloc);
        else if (easePartBehavior == EasePartBehavior::Mirror)
            val.AddMember("easePartBehavior", "Mirror", alloc);
        return val;
    }
} // namespace AdoCpp::Event::GamePlay
