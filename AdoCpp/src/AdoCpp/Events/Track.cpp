#include "Track.h"

#include "AdoCpp/Tile.h"


namespace AdoCpp::Event::Track
{
    ColorTrack::ColorTrack(const rapidjson::Value& data) : StaticEvent(data)
    {
        trackColor = Color(data["trackColor"].GetString());
        secondaryTrackColor = Color(data["secondaryTrackColor"].GetString());
        trackColorAnimDuration = data["trackColorAnimDuration"].GetDouble();
        trackColorType = cstr2trackColorType(data["trackColorType"].GetString());
        trackStyle = cstr2trackStyle(data["trackStyle"].GetString());
        if (data.HasMember("trackColorPulse"))
            trackColorPulse = cstr2trackColorPulse(data["trackColorPulse"].GetString());
        if (data.HasMember("trackPulseLength"))
            trackPulseLength = data["trackPulseLength"].GetUint();
        if (data.HasMember("trackTexture"))
            trackTexture = data["trackTexture"].GetString();
        if (data.HasMember("trackGlowIntensity"))
            trackGlowIntensity = data["trackGlowIntensity"].GetDouble();
    }
    rapidjson::Value ColorTrack::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);
        if (!active)
            val.AddMember("active", active, alloc);
        rapidjson::Value trackColorValue;
        const std::string trackColorStr = trackColor.toString(false, false, Color::ToStringAlphaMode::Auto);
        trackColorValue.SetString(trackColorStr.c_str(), trackColorStr.length(), alloc);
        val.AddMember("trackColor", trackColorValue, alloc);
        rapidjson::Value secondaryTrackColorValue;
        const std::string secondaryTrackColorStr = trackColor.toString(false, false, Color::ToStringAlphaMode::Auto);
        secondaryTrackColorValue.SetString(secondaryTrackColorStr.c_str(), secondaryTrackColorStr.length(),
                                           alloc);
        val.AddMember("secondaryTrackColor", secondaryTrackColorValue, alloc);
        if (static_cast<int>(trackColorAnimDuration) == trackColorAnimDuration)
            val.AddMember("trackColorAnimDuration", static_cast<int>(trackColorAnimDuration), alloc);
        else
            val.AddMember("trackColorAnimDuration", trackColorAnimDuration, alloc);
        val.AddMember("trackColorType", rapidjson::StringRef(trackColorType2cstr(trackColorType)), alloc);
        val.AddMember("trackColorPulse", rapidjson::StringRef(trackColorPulse2cstr(trackColorPulse)),
                      alloc);
        val.AddMember("trackPulseLength", trackPulseLength, alloc);
        val.AddMember("trackStyle", rapidjson::StringRef(trackStyle2cstr(trackStyle)), alloc);
        rapidjson::Value trackTextureValue;
        trackTextureValue.SetString(trackTexture.c_str(), trackTexture.length(), alloc);
        val.AddMember("trackTexture", trackTextureValue, alloc);
        if (static_cast<int>(trackGlowIntensity) == trackGlowIntensity)
            val.AddMember("trackGlowIntensity", static_cast<int>(trackGlowIntensity), alloc);
        else
            val.AddMember("trackGlowIntensity", trackGlowIntensity, alloc);
        return val;
    }
    PositionTrack::PositionTrack(const rapidjson::Value& data) : StaticEvent(data)
    {
        if (data.HasMember("positionOffset"))
        {
            const auto posOff = data["positionOffset"].GetArray();
            if (!posOff[0].IsNull())
                positionOffset.x = posOff[0].GetDouble();
            if (!posOff[1].IsNull())
                positionOffset.y = posOff[1].GetDouble();
        }
        relativeTo = data.HasMember("relativeTo") ? RelativeIndex(data["relativeTo"]) : RelativeIndex(0, ThisTile);
        rotation = data.HasMember("rotation") ? data["rotation"].GetDouble() : 0;
        scale = data.HasMember("scale") ? data["scale"].GetDouble() : 100;
        opacity = data.HasMember("opacity") ? data["opacity"].GetDouble() : 100;
        justThisTile = data.HasMember("justThisTile") ? toBool(data["justThisTile"]) : false;
        editorOnly = data.HasMember("editorOnly") ? toBool(data["editorOnly"]) : false;
        if (data.HasMember("stickToFloors"))
            stickToFloors = toBool(data["stickToFloors"]);
    }
    rapidjson::Value PositionTrack::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);
        if (!active)
            val.AddMember("active", active, alloc);
        rapidjson::Value posOffVal(rapidjson::kArrayType);
        if (static_cast<int>(positionOffset.x) == positionOffset.x)
            posOffVal.PushBack(static_cast<int>(positionOffset.x), alloc);
        else
            posOffVal.PushBack(positionOffset.x, alloc);
        if (static_cast<int>(positionOffset.y) == positionOffset.y)
            posOffVal.PushBack(static_cast<int>(positionOffset.y), alloc);
        else
            posOffVal.PushBack(positionOffset.y, alloc);
        val.AddMember("positionOffset", posOffVal, alloc);
        val.AddMember("relativeTo", relativeTo.intoJson(alloc), alloc);
        if (static_cast<int>(rotation) == rotation)
            val.AddMember("rotation", static_cast<int>(rotation), alloc);
        else
            val.AddMember("rotation", rotation, alloc);
        if (static_cast<int>(scale) == scale)
            val.AddMember("scale", static_cast<int>(scale), alloc);
        else
            val.AddMember("scale", scale, alloc);
        if (static_cast<int>(opacity) == opacity)
            val.AddMember("opacity", static_cast<int>(opacity), alloc);
        else
            val.AddMember("opacity", opacity, alloc);
        val.AddMember("justThisTile", justThisTile, alloc);
        val.AddMember("editorOnly", editorOnly, alloc);
        if (stickToFloors)
            val.AddMember("stickToFloors", *stickToFloors, alloc);
        return val;
    }
    MoveTrack::MoveTrack(const rapidjson::Value& data) : DynamicEvent(data)
    {
        startTile = RelativeIndex(data["startTile"]);
        endTile = RelativeIndex(data["endTile"]);
        duration = data["duration"].GetDouble();
        if (data.HasMember("positionOffset"))
        {
            if (!data["positionOffset"][0].IsNull())
                positionOffset.first = data["positionOffset"][0].GetDouble();
            if (!data["positionOffset"][1].IsNull())
                positionOffset.second = data["positionOffset"][1].GetDouble();
        }
        if (data.HasMember("rotationOffset"))
            rotationOffset = data["rotationOffset"].GetDouble();
        if (data.HasMember("scale"))
        {
            if (data["scale"].IsArray())
            {
                if (!data["scale"][0].IsNull())
                    scale.first = data["scale"][0].GetDouble();
                if (!data["scale"][1].IsNull())
                    scale.second = data["scale"][1].GetDouble();
            }
            else
            {
                scale.first = scale.second = data["scale"].GetDouble();
            }
        }
        if (data.HasMember("opacity"))
            opacity = data["opacity"].GetDouble();
        ease = cstr2easing(data["ease"].GetString());
    }
    rapidjson::Value MoveTrack::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);
        if (!active)
            val.AddMember("active", active, alloc);
        val.AddMember("startTile", startTile.intoJson(alloc), alloc);
        val.AddMember("endTile", endTile.intoJson(alloc), alloc);
        if (static_cast<int>(duration) == duration)
            val.AddMember("duration", static_cast<int>(duration), alloc);
        else
            val.AddMember("duration", duration, alloc);
        // ReSharper disable once CppLocalVariableMayBeConst
        if (auto op = optionalPoint2json(positionOffset, alloc); op)
            val.AddMember("positionOffset", *op, alloc);
        if (rotationOffset)
        {
            if (static_cast<int>(*rotationOffset) == *rotationOffset)
                val.AddMember("rotationOffset", static_cast<int>(*rotationOffset), alloc);
            else
                val.AddMember("rotationOffset", *rotationOffset, alloc);
        }
        if (auto op = optionalPoint2json(scale, alloc); op)
            val.AddMember("scale", *op, alloc);
        if (opacity)
        {
            if (static_cast<int>(*opacity) == *opacity)
                val.AddMember("opacity", static_cast<int>(*opacity), alloc);
            else
                val.AddMember("opacity", *opacity, alloc);
        }
        if (static_cast<int>(angleOffset) == angleOffset)
            val.AddMember("angleOffset", static_cast<int>(angleOffset), alloc);
        else
            val.AddMember("angleOffset", angleOffset, alloc);
        val.AddMember("ease", rapidjson::StringRef(easing2cstr(ease)), alloc);
        char tagBuf[1145]{};
        tags2cstr(eventTag, tagBuf, 1145);
        rapidjson::Value tagValue;
        tagValue.SetString(tagBuf, strlen(tagBuf), alloc);
        val.AddMember("eventTag", tagValue, alloc);
        return val;
    }
    AnimateTrack::AnimateTrack(const rapidjson::Value& data) : StaticEvent(data)
    {
        if (data.HasMember("trackAnimation"))
            trackAnimation = cstr2trackAnimation(data["trackAnimation"].GetString());
        beatsAhead = data["beatsAhead"].GetDouble();
        if (data.HasMember("trackDisappearAnimation"))
            trackDisappearAnimation = cstr2trackDisappearAnimation(data["trackDisappearAnimation"].GetString());
        beatsBehind = data["beatsBehind"].GetDouble();
    }
    rapidjson::Value AnimateTrack::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);
        if (!active)
            val.AddMember("active", active, alloc);
        if (trackAnimation)
            val.AddMember("trackAnimation", rapidjson::StringRef(trackAnimation2cstr(*trackAnimation)),
                          alloc);
        if (static_cast<int>(beatsAhead) == beatsAhead)
            val.AddMember("beatsAhead", static_cast<int>(beatsAhead), alloc);
        else
            val.AddMember("beatsAhead",beatsAhead, alloc);
        if (trackDisappearAnimation)
            val.AddMember("trackDisappearAnimation",
                          rapidjson::StringRef(trackDisappearAnimation2cstr(*trackDisappearAnimation)),
                          alloc);
        if (static_cast<int>(beatsBehind) == beatsBehind)
            val.AddMember("beatsBehind", static_cast<int>(beatsBehind), alloc);
        else
            val.AddMember("beatsBehind", beatsBehind, alloc);
        return val;
    }
    RecolorTrack::RecolorTrack(const rapidjson::Value& data) : DynamicEvent(data)
    {
        startTile = RelativeIndex(data["startTile"]);
        endTile = RelativeIndex(data["endTile"]);
        if (data.HasMember("duration"))
            duration = data["duration"].GetDouble();
        trackColorType = cstr2trackColorType(data["trackColorType"].GetString());
        trackColor = Color(data["trackColor"].GetString());
        secondaryTrackColor = Color(data["secondaryTrackColor"].GetString());
        trackColorAnimDuration = data["trackColorAnimDuration"].GetDouble();
        trackColorPulse = cstr2trackColorPulse(data["trackColorPulse"].GetString());
        trackPulseLength = data["trackPulseLength"].GetUint();
        trackStyle = cstr2trackStyle(data["trackStyle"].GetString());
        if (data.HasMember("gapLength"))
            gapLength = data["gapLength"].GetDouble();
        if (data.HasMember("ease"))
            ease = cstr2easing(data["ease"].GetString());
        if (data.HasMember("trackGlowIntensity"))
            trackGlowIntensity = data["trackGlowIntensity"].GetDouble();
    }
    rapidjson::Value RecolorTrack::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value val(rapidjson::kObjectType);
        val.AddMember("floor", floor, alloc);
        val.AddMember("eventType", rapidjson::StringRef(name()), alloc);
        if (!active)
            val.AddMember("active", active, alloc);
        val.AddMember("startTile", startTile.intoJson(alloc), alloc);
        val.AddMember("endTile", endTile.intoJson(alloc), alloc);
        if (duration)
        {
            if (static_cast<int>(*duration) == *duration)
                val.AddMember("duration", static_cast<int>(*duration), alloc);
            else
                val.AddMember("duration", *duration, alloc);
        }
        val.AddMember("trackColorType", rapidjson::StringRef(trackColorType2cstr(trackColorType)), alloc);
        rapidjson::Value trackColorValue;
        const std::string trackColorStr = trackColor.toString(false, false, Color::ToStringAlphaMode::Auto);
        trackColorValue.SetString(trackColorStr.c_str(), trackColorStr.length(), alloc);
        val.AddMember("trackColor", trackColorValue, alloc);
        rapidjson::Value secondaryTrackColorValue;
        const std::string secondaryTrackColorStr = trackColor.toString(false, false, Color::ToStringAlphaMode::Auto);
        secondaryTrackColorValue.SetString(secondaryTrackColorStr.c_str(), secondaryTrackColorStr.length(),
                                           alloc);
        val.AddMember("secondaryTrackColor", secondaryTrackColorValue, alloc);
        if (static_cast<int>(trackColorAnimDuration) == trackColorAnimDuration)
            val.AddMember("trackColorAnimDuration", static_cast<int>(trackColorAnimDuration), alloc);
        else
            val.AddMember("trackColorAnimDuration", trackColorAnimDuration, alloc);
        val.AddMember("trackColorPulse", rapidjson::StringRef(trackColorPulse2cstr(trackColorPulse)),
                      alloc);
        val.AddMember("trackPulseLength", trackPulseLength, alloc);
        val.AddMember("trackStyle", rapidjson::StringRef(trackStyle2cstr(trackStyle)), alloc);
        if (static_cast<int>(gapLength) == gapLength)
            val.AddMember("gapLength", static_cast<int>(gapLength), alloc);
        else
            val.AddMember("gapLength", gapLength, alloc);
        val.AddMember("ease", rapidjson::StringRef(easing2cstr(ease)), alloc);
        if (static_cast<int>(trackGlowIntensity) == trackGlowIntensity)
            val.AddMember("trackGlowIntensity", static_cast<int>(trackGlowIntensity), alloc);
        else
            val.AddMember("trackGlowIntensity", trackGlowIntensity, alloc);
        char tagBuf[1145]{};
        tags2cstr(eventTag, tagBuf, 1145);
        rapidjson::Value tagValue;
        tagValue.SetString(tagBuf, strlen(tagBuf), alloc);
        val.AddMember("eventTag", tagValue, alloc);
        if (static_cast<int>(angleOffset) == angleOffset)
            val.AddMember("angleOffset", static_cast<int>(angleOffset), alloc);
        else
            val.AddMember("angleOffset", angleOffset, alloc);
        return val;
    }
} // namespace AdoCpp::Event::Track
