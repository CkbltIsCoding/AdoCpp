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
        if (data.HasMember("trackTexture"))
            trackTexture = data["trackTexture"].GetString();
        if (data.HasMember("trackGlowIntensity"))
            trackGlowIntensity = data["trackGlowIntensity"].GetDouble();
    }
    PositionTrack::PositionTrack(const rapidjson::Value& data) : StaticEvent(data)
    {
        if (data.HasMember("positionOffset"))
        {
            auto posOff = data["positionOffset"].GetArray();
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
    AnimateTrack::AnimateTrack(const rapidjson::Value& data) : StaticEvent(data)
    {
        if (data.HasMember("trackAnimation"))
            trackAnimation = cstr2trackAnimation(data["trackAnimation"].GetString());
        beatsAhead = data["beatsAhead"].GetDouble();
        if (data.HasMember("trackDisappearAnimation"))
            trackDisappearAnimation = cstr2trackDisappearAnimation(data["trackDisappearAnimation"].GetString());
        beatsBehind = data["beatsBehind"].GetDouble();
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
        trackPulseLength = data["trackPulseLength"].GetDouble();
        trackStyle = cstr2trackStyle(data["trackStyle"].GetString());
    }
} // namespace AdoCpp::Event::Track
