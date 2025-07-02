#include "Event.h"
#include <sstream>
#include "Level.h"

namespace AdoCpp
{
    Event::Event* Event::newEvent(const rapidjson::Value& json)
    {
        const std::string eventType = json["eventType"].GetString();
        if (eventType == "SetSpeed")
            return new GamePlay::SetSpeed(json);
        if (eventType == "Twirl")
            return new GamePlay::Twirl(json);
        if (eventType == "Pause")
            return new GamePlay::Pause(json);
        if (eventType == "SetPlanetRotation")
            return new GamePlay::SetPlanetRotation(json);

        if (eventType == "ColorTrack")
            return new Track::ColorTrack(json);
        if (eventType == "AnimateTrack")
            return new Track::AnimateTrack(json);
        if (eventType == "RecolorTrack")
            return new Track::RecolorTrack(json);
        if (eventType == "PositionTrack")
            return new Track::PositionTrack(json);
        if (eventType == "MoveTrack")
            return new Track::MoveTrack(json);

        if (eventType == "MoveCamera")
            return new Visual::MoveCamera(json);

        if (eventType == "RepeatEvents")
            return new Modifiers::RepeatEvents(json);

        if (eventType == "Hold")
            return new Dlc::Hold(json);

        return nullptr;
    }
} // namespace AdoCpp
