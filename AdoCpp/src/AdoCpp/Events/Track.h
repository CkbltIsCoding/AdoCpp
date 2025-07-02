#pragma once
#include "AdoCpp/Color.h"
#include "AdoCpp/Easing.h"
#include "AdoCpp/Tile.h"
#include "AdoCpp/Utils.h"
#include "Base.h"

namespace AdoCpp::Event::Track
{
    class ColorTrack final : public StaticEvent
    {
    public:
        ColorTrack() = default;
        explicit ColorTrack(const rapidjson::Value& data);
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
    class AnimateTrack final : public StaticEvent
    {
    public:
        AnimateTrack() = default;
        explicit AnimateTrack(const rapidjson::Value& data);
        bool stackable() override { return false; }
        std::string name() override { return "AnimateTrack"; }
        AnimateTrack* clone() override { return new AnimateTrack(*this); }
        std::optional<TrackAnimation> trackAnimation;
        double beatsAhead;
        std::optional<TrackDisappearAnimation> trackDisappearAnimation;
        double beatsBehind;
    };
    class RecolorTrack final : public DynamicEvent
    {
    public:
        RecolorTrack() = default;
        explicit RecolorTrack(const rapidjson::Value& data);
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
    class PositionTrack final : public StaticEvent
    {
    public:
        PositionTrack() = default;
        explicit PositionTrack(const rapidjson::Value& data);
        bool stackable() override { return false; }
        std::string name() override { return "PositionTrack"; }
        PositionTrack* clone() override { return new PositionTrack(*this); }
        Vector2lf positionOffset;
        RelativeIndex relativeTo;
        double rotation;
        double scale;
        double opacity;
        bool justThisTile;
        bool editorOnly;
        std::optional<bool> stickToFloors;
    };
    class MoveTrack final : public DynamicEvent
    {
    public:
        MoveTrack() = default;
        explicit MoveTrack(const rapidjson::Value& data);
        bool stackable() override { return true; }
        std::string name() override { return "MoveTrack"; }
        MoveTrack* clone() override { return new MoveTrack(*this); }
        RelativeIndex startTile;
        RelativeIndex endTile;
        double duration = 0;
        OptionalPoint positionOffset;
        std::optional<double> rotationOffset;
        OptionalPoint scale;
        std::optional<double> opacity;
        Easing ease = Easing::Linear;
    };
} // namespace AdoCpp::Event::Track
