#include "Event.h"
#include "Level.h"
#include <sstream>

namespace AdoCpp
{
	std::map<TrackColorType, std::string> trackColorType2string = {
		{TrackColorType::Single, "Single"},
		{TrackColorType::Stripes, "Stripes"},
		{TrackColorType::Glow, "Glow"},
		{TrackColorType::Blink, "Blink"},
		{TrackColorType::Switch, "Switch"},
		{TrackColorType::Rainbow, "Rainbow"},
		{TrackColorType::Volume, "Volume"},
	};
	std::map<std::string, TrackColorType> string2trackColorType = {
		{"Single", TrackColorType::Single},
		{"Stripes", TrackColorType::Stripes},
		{"Glow", TrackColorType::Glow},
		{"Blink", TrackColorType::Blink},
		{"Switch", TrackColorType::Switch},
		{"Rainbow", TrackColorType::Rainbow},
		{"Volume", TrackColorType::Volume},
	};
	std::map<TrackStyle, std::string> trackStyle2string = {
		{TrackStyle::Standard, "Standard"},
		{TrackStyle::Neon, "Neon"},
		{TrackStyle::NeonLight, "NeonLight"},
		{TrackStyle::Basic, "Basic"},
		{TrackStyle::Minimal, "Minimal"},
		{TrackStyle::Gems, "Gems"},
	};
	std::map<std::string, TrackStyle> string2trackStyle = {
		{"Standard", TrackStyle::Standard},
		{"Neon", TrackStyle::Neon},
		{"NeonLight", TrackStyle::NeonLight},
		{"Basic", TrackStyle::Basic},
		{"Minimal", TrackStyle::Minimal},
		{"Gems", TrackStyle::Gems},
	};
	std::map<std::string, TrackColorPulse> string2trackColorPulse = {
		{"None", TrackColorPulse::None},
		{"Forward", TrackColorPulse::Forward},
		{"Backward", TrackColorPulse::Backward},
	};
	std::map<std::string, TrackAnimation> string2TrackAnimation = {
		{"None", TrackAnimation::None},
		{"Fade", TrackAnimation::Fade},
	};

	static std::vector<std::string> stringSplit(std::string str, const char split)
	{
		std::istringstream iss(str);
		std::string token;
		std::vector<std::string> vec;
		while (getline(iss, token, split))
			vec.push_back(token);
		return vec;
	}

	namespace Event
	{
		Event::Event(const rapidjson::Value& data)
		{
			this->floor = data["floor"].GetUint64();
		}
		BeatEvent::BeatEvent(const rapidjson::Value& data)
			: Event(data)
		{
			if (data.HasMember("angleOffset"))
				angleOffset = data["angleOffset"].GetDouble();
			if (data.HasMember("eventTag"))
				eventTag = stringSplit(data["eventTag"].GetString(), ' ');
		}
		namespace GamePlay
		{
			SetSpeed::SetSpeed(const rapidjson::Value& data)
				: BeatEvent(data)
			{
				beatsPerMinute = data["beatsPerMinute"].GetDouble();
				if (!data.HasMember("speedType"))
				{
					speedType = SpeedType::Bpm;
					return;
				}
				bpmMultiplier = data["bpmMultiplier"].GetDouble();
				if (!strcmp(data["speedType"].GetString(), "Bpm"))
					speedType = SpeedType::Bpm;
				else
					speedType = SpeedType::Multiplier;
			}
			Twirl::Twirl(const rapidjson::Value& data)
				: Event(data)
			{
			}
			Pause::Pause(const rapidjson::Value& data)
				: Event(data)
			{
				duration = data["duration"].GetDouble();
				countdownTicks = data["countdownTicks"].GetDouble();
				angleCorrectionDir = data["angleCorrectionDir"].GetInt();
			}
		}
		namespace Track
		{
			ColorTrack::ColorTrack(const rapidjson::Value& data)
				: Event(data)
			{
				trackColor = Color(data["trackColor"].GetString());
				secondaryTrackColor = Color(data["secondaryTrackColor"].GetString());
				trackColorAnimDuration = data["trackColorAnimDuration"].GetDouble();
				trackColorType = string2trackColorType[data["trackColorType"].GetString()];
				trackStyle = string2trackStyle[data["trackStyle"].GetString()];
				if (data.HasMember("trackTexture"))
					trackTexture = data["trackTexture"].GetString();
				if (data.HasMember("trackGlowIntensity"))
					trackGlowIntensity = data["trackGlowIntensity"].GetDouble();
			}
			PositionTrack::PositionTrack(const rapidjson::Value& data)
				: Event(data)
			{
				if (data.HasMember("positionOffset"))
				{
					auto posOff = data["positionOffset"].GetArray();
					if (!posOff[0].IsNull()) positionOffset.first = posOff[0].GetDouble();
					if (!posOff[1].IsNull()) positionOffset.second = posOff[1].GetDouble();
				}
				relativeTo = data.HasMember("relativeTo") ? toRelativeIndex(data["relativeTo"]) : RelativeIndex(0, "ThisTile");
				rotation = data.HasMember("rotation") ? data["rotation"].GetDouble() : 0;
				scale = data.HasMember("scale") ? data["scale"].GetDouble() : 100;
				opacity = data.HasMember("opacity") ? data["opacity"].GetDouble() : 100;
				justThisTile = data.HasMember("justThisTile") ? toBool(data["justThisTile"]) : false;
				editorOnly = data.HasMember("editorOnly") ? toBool(data["editorOnly"]) : false;
				if (data.HasMember("stickToFloors"))
					stickToFloors = toBool(data["stickToFloors"]);
			}
			MoveTrack::MoveTrack(const rapidjson::Value& data)
				: BeatEvent(data)
			{
				startTile = toRelativeIndex(data["startTile"]);
				endTile = toRelativeIndex(data["endTile"]);
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
				ease = string2easing[data["ease"].GetString()];
			}
			AnimateTrack::AnimateTrack(const rapidjson::Value& data)
				: Event(data)
			{
				// TODO FIXME
				if (data.HasMember("trackAnimation"))
					trackAnimation = !strcmp(data["trackAnimation"].GetString(), "Fade")
						? TrackAnimation::Fade : TrackAnimation::None;
				beatsAhead = data["beatsAhead"].GetDouble();
				if (data.HasMember("trackDisappearAnimation"))
					trackDisappearAnimation = !strcmp(data["trackDisappearAnimation"].GetString(), "Fade")
						? TrackAnimation::Fade : TrackAnimation::None;
				beatsBehind = data["beatsBehind"].GetDouble();
			}
			RecolorTrack::RecolorTrack(const rapidjson::Value& data)
				: BeatEvent(data)
			{
				startTile = toRelativeIndex(data["startTile"]);
				endTile = toRelativeIndex(data["endTile"]);
				if (data.HasMember("duration"))
					duration = data["duration"].GetDouble();
				trackColorType = string2trackColorType[data["trackColorType"].GetString()];
				trackColor = Color(data["trackColor"].GetString());
				secondaryTrackColor = Color(data["secondaryTrackColor"].GetString());
				trackColorAnimDuration = data["trackColorAnimDuration"].GetDouble();
				trackColorPulse = string2trackColorPulse[data["trackColorPulse"].GetString()];
				trackPulseLength = data["trackPulseLength"].GetDouble();
				trackStyle = string2trackStyle[data["trackStyle"].GetString()];
			}
		}
		namespace Visual
		{
			MoveCamera::MoveCamera(const rapidjson::Value& data)
				: BeatEvent(data)
			{
				duration = data["duration"].GetDouble();
				if (data.HasMember("relativeTo"))
					relativeTo = data["relativeTo"].GetString();
				if (data.HasMember("position"))
				{
					if (!data["position"][0].IsNull()) position.first = data["position"][0].GetDouble();
					if (!data["position"][1].IsNull()) position.second = data["position"][1].GetDouble();
				}
				if (data.HasMember("rotation"))
					rotation = data["rotation"].GetDouble();
				if (data.HasMember("zoom"))
					zoom = data["zoom"].GetDouble();
				ease = string2easing[data["ease"].GetString()];
			}
		}
		namespace Modifiers
		{
			RepeatEvents::RepeatEvents(const rapidjson::Value& data)
				: Event(data)
			{
				if (data.HasMember("repeatType") && !strcmp(data["repeatType"].GetString(), "Floor"))
					repeatType = RepeatType::Floor;
				else
					repeatType = RepeatType::Beat;
				repetitions = data["repetitions"].GetUint64();
				floorCount = data.HasMember("floorCount") ? data["floorCount"].GetUint64() : 0;
				interval = data["interval"].GetDouble();
				executeOnCurrentFloor = data.HasMember("executeOnCurrentFloor")
					? toBool(data["executeOnCurrentFloor"]) : false;
				tag = stringSplit(data["tag"].GetString(), ' ');
			}
		}
	}
}
