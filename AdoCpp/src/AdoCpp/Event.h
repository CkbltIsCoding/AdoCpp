#pragma once
#include <map>
#include <optional>
#include <vector>
#include <rapidjson/document.h>

#include "Utils.h"
#include "Easing.h"

namespace AdoCpp
{
	extern std::map<char, double> path2angle;

	enum class TrackColorType
	{
		Single,
		Stripes,
		Glow,
		Blink,
		Switch,
		Rainbow,
		Volume,
	};
	extern std::map<TrackColorType, std::string> trackColorType2string;
	extern std::map<std::string, TrackColorType> string2trackColorType;
	
	enum class TrackStyle
	{
		Standard,
		Neon,
		NeonLight,
		Basic,
		Minimal,
		Gems,
	};
	extern std::map<TrackStyle, std::string> trackStyle2string;
	extern std::map<std::string, TrackStyle> string2trackStyle;
	
	enum class TrackAnimation
	{ // TODO
	};
	extern std::map<TrackAnimation, std::string> trackAnimation2string;
	extern std::map<std::string, TrackAnimation> string2trackAnimation;

	enum class TrackColorPulse
	{
		None,
		Forward,
		Backward
	};
	extern std::map<TrackColorPulse, std::string> trackColorPulse2string;
	extern std::map<std::string, TrackColorPulse> string2trackColorPulse;

	/**
	 * @brief Event namespace.
	 */
	namespace Event
	{
		class Event
		{
		public:
			Event() = default;
			Event(const rapidjson::Value& data);
			size_t floor = 0;
			virtual bool stackable() = 0;
			virtual std::string name() = 0;
			virtual Event* clone() = 0;
		};
		class BeatEvent : public Event
		{
		public:
			BeatEvent() = default;
			BeatEvent(const rapidjson::Value& data);
			virtual BeatEvent* clone() = 0;
			double angleOffset = 0;
			double beat = 0;
			std::vector<std::string> eventTag;
			bool repeat = false;
		};
		namespace GamePlay
		{
			class SetSpeed : public BeatEvent
			{
			public:
				enum class SpeedType
				{
					Bpm, Multiplier
				};
				SetSpeed() = default;
				SetSpeed(const rapidjson::Value& data);
				bool stackable() override { return true; }
				std::string name() override { return "SetSpeed"; }
				SetSpeed* clone() override { return new SetSpeed(*this); }
				SpeedType speedType = SpeedType::Bpm;
				double beatsPerMinute = 100;
				double bpmMultiplier = 1;
			private:
			};
			class Twirl : public Event
			{
			public:
				Twirl() = default;
				Twirl(const rapidjson::Value& data);
				bool stackable() override { return false; }
				std::string name() override { return "Twirl"; }
				Twirl* clone() override { return new Twirl(*this); }
			};
			class Pause : public Event
			{
			public:
				Pause() = default;
				Pause(const rapidjson::Value& data);
				bool stackable() override { return false; };
				std::string name() override { return "Pause"; };
				Pause* clone() override { return new Pause(*this); }
				double duration;
				double countdownTicks;
				int angleCorrectionDir;
			};
		}
		namespace Track
		{
			class ColorTrack : public Event
			{
			public:
				ColorTrack() = default;
				ColorTrack(const rapidjson::Value& data);
				bool stackable() override { return false; }
				std::string name() override { return "ColorTrack"; }
				ColorTrack* clone() override { return new ColorTrack(*this); }
				TrackColorType trackColorType;
				Color trackColor;
				Color secondaryTrackColor;
				double trackColorAnimDuration;
				TrackStyle trackStyle;
				std::string trackTexture;
				double trackGlowIntensity;
			};
			class AnimateTrack : public Event
			{
			public:
				AnimateTrack() = default;
				AnimateTrack(const rapidjson::Value& data);
				bool stackable() override { return false; }
				std::string name() override { return "AnimateTrack"; }
				AnimateTrack* clone() override { return new AnimateTrack(*this); }
				TrackAnimation trackAnimation;
				double beatsAhead;
				TrackAnimation trackDisappearAnimation;
				double beatsBehind;
			};
			class RecolorTrack : public BeatEvent
			{
			public:
				RecolorTrack() = default;
				RecolorTrack(const rapidjson::Value& data);
				bool stackable() override { return true; }
				std::string name() override { return "RecolorTrack"; }
				RecolorTrack* clone() override { return new RecolorTrack(*this); }
				RelativeIndex startTile;
				RelativeIndex endTile;
				double gapLength;
				std::optional<double> duration;
				TrackColorType trackColorType;
				Color trackColor;
				Color secondaryTrackColor;
				double trackColorAnimDuration;
				TrackColorPulse trackColorPulse;
				double trackPulseLength;
				TrackStyle trackStyle;
				double trackGlowIntensity;
				Easing ease;
			};
			class PositionTrack : public Event
			{
			public:
				PositionTrack() = default;
				PositionTrack(const rapidjson::Value& data);
				bool stackable() override { return false; }
				std::string name() override { return "PositionTrack"; }
				PositionTrack* clone() override { return new PositionTrack(*this); }
				Point positionOffset;
				RelativeIndex relativeTo;
				double rotation;
				double scale;
				double opacity;
				bool justThisTile;
				bool editorOnly;
				std::optional<bool> stickToFloors;
			};
			class MoveTrack : public BeatEvent
			{
			public:
				MoveTrack() = default;
				MoveTrack(const rapidjson::Value& data);
				bool stackable() override { return true; }
				std::string name() override { return "MoveTrack"; }
				MoveTrack* clone() override { return new MoveTrack(*this); }
				RelativeIndex startTile;
				RelativeIndex endTile;
				double duration = 1;
				OptionalPoint positionOffset;
				std::optional<double> rotationOffset = 0.0;
				OptionalPoint scale;
				std::optional<double> opacity;
				Easing ease;
			};
		}
		namespace Visual
		{
			class MoveCamera : public BeatEvent
			{
			public:
				MoveCamera() = default;
				MoveCamera(const rapidjson::Value& data);
				bool stackable() override { return true; }
				std::string name() override { return "MoveCamera"; }
				MoveCamera* clone() override { return new MoveCamera(*this); }
				double duration = 1;
				std::optional<std::string> relativeTo;
				OptionalPoint position;
				std::optional<double> rotation = 0.0;
				std::optional<double> zoom;
				Easing ease;
			};
		}
		namespace Modifiers
		{
			class RepeatEvents : public Event
			{
			public:
				enum class RepeatType
				{
					Beat, Floor
				};
				RepeatEvents() = default;
				RepeatEvents(const rapidjson::Value& data);
				bool stackable() override { return true; }
				std::string name() override { return "RepeatEvents"; }
				RepeatEvents* clone() override { return new RepeatEvents(*this); }
				RepeatType repeatType;
				size_t repetitions;
				size_t floorCount;
				double interval;
				bool executeOnCurrentFloor;
				std::vector<std::string> tag;
				double duration = 1;
			};
		}
	}
}

