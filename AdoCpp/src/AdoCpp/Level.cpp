#include "Level.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <ranges>
#include <tuple>

#include "Utils.h"

namespace AdoCpp
{
    // clang-format off
    constexpr double angles[] = {
          0,  15,  30,  45,  60,  75,
         90, 105, 120, 135, 150, 165,
        180, 195, 210, 225, 240, 255,
        270, 285, 300, 315, 330, 345,
        555, 666, 777, 888, 999
    };
    constexpr char paths[] = {
        'R', 'p', 'J', 'E', 'T', 'o',
        'U', 'q', 'G', 'Q', 'H', 'W',
        'L', 'x', 'N', 'Z', 'F', 'V',
        'D', 'Y', 'B', 'C', 'M', 'A',
        '5', '6', '7', '8', '9'
    };
    // clang-format on
    constexpr double path2angle(const char path)
    {
        for (size_t i = 0; i < std::size(angles); ++i)
            if (path == paths[i])
                return angles[i];
        throw std::invalid_argument("Invalid path");
    }

    Level::Level(const rapidjson::Document& document) { fromJson(document); }

    Level::Level(std::ifstream& ifs) { fromFile(ifs); }

    Level::Level(const std::filesystem::path& path) { fromFile(path); }

    Level::~Level()
    {
        while (!m_processedDynamicEvents.empty())
        {
            if (m_processedDynamicEvents.back()->generated)
                delete m_processedDynamicEvents.back();
            m_processedDynamicEvents.pop_back();
        }
        while (!events.empty())
        {
            delete events.back();
            events.pop_back();
        }
    }

    static double deg2rad(const double deg) { return deg * 3.141592653589793 / 180; }

    void Level::clear()
    {
        settings = Settings();
        tiles.clear();
        while (!m_processedDynamicEvents.empty())
        {
            if (m_processedDynamicEvents.back()->generated)
                delete m_processedDynamicEvents.back();
            m_processedDynamicEvents.pop_back();
        }
        while (!events.empty())
        {
            delete events.back();
            events.pop_back();
        }
        m_moveCameraDatas.clear();
        m_setSpeeds.clear();
        parsed = false;
    }

    void Level::defaultLevel()
    {
        clear();
        tiles.emplace_back(0);
        tiles[0].beat = -std::numeric_limits<double>::infinity();
        for (size_t i = 0; i < 10; i++)
            tiles.emplace_back(0);
        parse();
        update();
    }

    void Level::fromJson(const rapidjson::Document& document)
    {
        parsed = false;
        auto s = document["settings"].GetObject();
        settings.bpm = s["bpm"].GetDouble();
        settings.offset = s["offset"].GetDouble();
        settings.songFilename = s["songFilename"].GetString();
        settings.countdownTicks = s["countdownTicks"].GetDouble();
        settings.trackColorType = cstr2trackColorType(s["trackColorType"].GetString());
        settings.trackColor = Color(s["trackColor"].GetString());
        settings.secondaryTrackColor = Color(s["secondaryTrackColor"].GetString());
        settings.trackColorAnimDuration = s["trackColorAnimDuration"].GetDouble();
        settings.trackColorPulse = cstr2trackColorPulse(s["trackColorPulse"].GetString());
        settings.trackPulseLength = s["trackPulseLength"].GetDouble();
        settings.trackStyle = cstr2trackStyle(s["trackStyle"].GetString());
        settings.trackAnimation = cstr2trackAnimation(s["trackAnimation"].GetString());
        settings.beatsAhead = s["beatsAhead"].GetDouble();
        settings.trackDisappearAnimation = cstr2trackDisappearAnimation(s["trackDisappearAnimation"].GetString());
        settings.beatsBehind = s["beatsBehind"].GetDouble();
        settings.backgroundColor = Color(s["backgroundColor"].GetString());
        settings.stickToFloors = toBool(s["stickToFloors"]);
        if (s.HasMember("unscaledSize"))
            settings.unscaledSize = s["unscaledSize"].GetDouble();
        settings.relativeTo = cstr2relativeToCamera(s["relativeTo"].GetString());
        settings.position = Vector2lf(s["position"][0].GetDouble(), s["position"][1].GetDouble());
        settings.rotation = s["rotation"].GetDouble();
        settings.zoom = s["zoom"].GetDouble();

        tiles.emplace_back(0);
        if (document.HasMember("angleData"))
        {
            for (const auto& angle : document["angleData"].GetArray())
                tiles.emplace_back(angle.GetDouble());
        }
        else
        {
            if (!document.HasMember("pathData"))
                throw LevelPathAngleDataNotFoundException();
            for (const auto& path : std::string(document["pathData"].GetString()))
                tiles.emplace_back(path2angle(path));
        }

        for (const auto& eventData : document["actions"].GetArray())
        {
            if (Event::Event* event = Event::newEvent(eventData))
                events.push_back(event);
        }
    }

    void Level::fromFile(std::ifstream& ifs)
    {
        parsed = false;
        rapidjson::Document document;
        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::AutoUTFInputStream<unsigned, rapidjson::IStreamWrapper> eis(isw);
        document.ParseStream<rapidjson::kParseValidateEncodingFlag | rapidjson::kParseCommentsFlag |
                                 rapidjson::kParseTrailingCommasFlag | rapidjson::kParseNanAndInfFlag
                             //| rapidjson::kParseFullPrecisionFlag
                             ,
                             rapidjson::AutoUTF<unsigned>>(eis);
        if (document.HasParseError())
        {
            throw LevelJsonException(document.GetParseError());
        }
        fromJson(document);
    }
    void Level::fromFile(const std::filesystem::path& path)
    {
        parsed = false;
        std::ifstream ifs(path);
        if (!ifs.is_open())
            throw LevelCouldNotOpenFileException();
        fromFile(ifs);
        ifs.close();
    }

    void Level::parse()
    {
        if (parsed)
            return;
        if (tiles.size() <= 1)
        {
            parsed = false;
            return;
        }
        parsed = true;
        std::vector<bool> twirls(tiles.size());
        std::vector<double> pauses(tiles.size());
        std::vector<std::optional<Event::Track::PositionTrack>> positionTracks(tiles.size());
        std::vector<std::optional<Event::Track::ColorTrack>> colorTracks(tiles.size());
        std::vector<std::optional<Event::Track::AnimateTrack>> animateTracks(tiles.size());
        std::vector<std::optional<Event::Dlc::Hold>> holds(tiles.size());
        for (const auto& event : events)
        {
            if (typeid(*event) == typeid(Event::GamePlay::Twirl))
                twirls[event->floor] = true;
            else if (const auto pause = dynamic_cast<Event::GamePlay::Pause*>(event))
                pauses[pause->floor] = pause->duration;

            else if (const auto positionTrack = dynamic_cast<Event::Track::PositionTrack*>(event))
                positionTracks[positionTrack->floor] =
                    (std::make_optional<Event::Track::PositionTrack>(*positionTrack));
            else if (const auto colorTrack = dynamic_cast<Event::Track::ColorTrack*>(event))
                colorTracks[colorTrack->floor] = (std::make_optional<Event::Track::ColorTrack>(*colorTrack));
            else if (const auto animateTrack = dynamic_cast<Event::Track::AnimateTrack*>(event))
                animateTracks[animateTrack->floor] = (std::make_optional<Event::Track::AnimateTrack>(*animateTrack));
            else if (const auto hold = dynamic_cast<Event::Dlc::Hold*>(event))
                holds[hold->floor] = std::make_optional<Event::Dlc::Hold>(*hold);
        }
        tiles[0].orbit = Clockwise, tiles[0].beat = 0, settings.apply(tiles[0]);
        for (size_t i = 0; i < tiles.size(); i++)
        {
            // Tile's twirl
            if (i != 0)
                tiles[i].orbit = tiles[i - 1].orbit;
            if (twirls[i])
                tiles[i].orbit = !tiles[i].orbit;

            // Tile's beat
            if (i != 0)
            {
                if (tiles[i].angle.deg() == 999)
                {
                    tiles[i].beat = tiles[i - 1].beat;
                }
                else
                {
                    double angle, beat;
                    if (tiles[i - 1].angle.deg() == 999)
                        angle = tiles[i - 2].angle.deg() - tiles[i].angle.deg();
                    else
                        angle = tiles[i - 1].angle.deg() - 180 - tiles[i].angle.deg();
                    if (tiles[i - 1].orbit == CounterClockwise)
                        angle *= -1;
                    while (angle <= 0)
                        angle += 360;
                    while (angle > 360)
                        angle -= 360;
                    if (i == 1)
                        angle -= 180;
                    beat = angle / 180 + pauses[i - 1] + (holds[i - 1] ? holds[i - 1]->duration * 2 : 0);
                    tiles[i].beat = tiles[i - 1].beat + beat;
                }
            }

            // Tile's position
            if (i != 0)
            {
                tiles[i].stickToFloors = tiles[i - 1].stickToFloors;
                tiles[i].pos.o += tiles[i - 1].pos.o, tiles[i].editorPos = tiles[i - 1].editorPos;
                if ((i + 1 == tiles.size() || tiles[i + 1].angle.deg() != 999) && tiles[i].angle.deg() != 999)
                    tiles[i].pos.o.x += cos(deg2rad(tiles[i].angle.deg())),
                        tiles[i].editorPos.x += cos(deg2rad(tiles[i].angle.deg())),
                        tiles[i].pos.o.y += sin(deg2rad(tiles[i].angle.deg())),
                        tiles[i].editorPos.y += sin(deg2rad(tiles[i].angle.deg()));
            }
            if (positionTracks[i])
            {
                tiles[i].editorPos += positionTracks[i]->positionOffset;
                if (positionTracks[i]->justThisTile && i != tiles.size() - 1)
                {
                    tiles[i + 1].pos.o -= positionTracks[i]->positionOffset;
                }
                if (!positionTracks[i]->editorOnly)
                {
                    tiles[i].pos.o += positionTracks[i]->positionOffset;
                }
                if (positionTracks[i]->stickToFloors)
                    tiles[i].stickToFloors = *positionTracks[i]->stickToFloors;
            }

            // Tile's color
            if (i != 0)
            {
                tiles[i].trackColor.o = tiles[i - 1].trackColor.o,
                tiles[i].secondaryTrackColor.o = tiles[i - 1].secondaryTrackColor.o,
                tiles[i].trackStyle.o = tiles[i - 1].trackStyle.o;
            }
            if (colorTracks[i])
            {
                tiles[i].trackColor.o = colorTracks[i]->trackColor,
                tiles[i].secondaryTrackColor.o = colorTracks[i]->secondaryTrackColor,
                tiles[i].trackStyle.o = colorTracks[i]->trackStyle;
            }

            // Tile's animation
            if (i != 0)
            {
                tiles[i].trackAnimationFloor = tiles[i - 1].trackAnimationFloor;
                tiles[i].trackAnimation = tiles[i - 1].trackAnimation;
                tiles[i].beatsAhead = tiles[i - 1].beatsAhead;
                tiles[i].trackDisappearAnimation = tiles[i - 1].trackDisappearAnimation;
                tiles[i].beatsBehind = tiles[i - 1].beatsBehind;
            }
            if (animateTracks[i])
            {
                tiles[i].trackAnimationFloor = animateTracks[i]->floor;
                if (animateTracks[i]->trackAnimation)
                    tiles[i].trackAnimation = *animateTracks[i]->trackAnimation;
                tiles[i].beatsAhead = animateTracks[i]->beatsAhead;
                if (animateTracks[i]->trackDisappearAnimation)
                    tiles[i].trackDisappearAnimation = *animateTracks[i]->trackDisappearAnimation;
                tiles[i].beatsBehind = animateTracks[i]->beatsBehind;
            }

            // Tile's events
            tiles[i].events.clear();
        }
        tiles[0].beat = -settings.countdownTicks;
        std::vector<AdoCpp::Event::DynamicEvent*> dynamicEvents;
        m_processedDynamicEvents.clear();
        std::vector<std::vector<Event::GamePlay::SetSpeed*>> vecSetSpeed{tiles.size()};
        std::vector<std::vector<Event::Modifiers::RepeatEvents*>> vecRe{tiles.size()};
        m_setSpeeds.clear();
        for (const auto& event : events)
        {
            if (const auto setSpeed = dynamic_cast<Event::GamePlay::SetSpeed*>(event))
                setSpeed->beat = tiles[setSpeed->floor].beat + setSpeed->angleOffset / 180,
                m_setSpeeds.push_back(setSpeed), vecSetSpeed[setSpeed->floor].push_back(setSpeed);
            tiles[event->floor].events.push_back(event);
        }
        for (auto& tile : tiles)
            tile.seconds = beat2seconds(tile.beat);
        for (const auto& event : events)
        {
            if (auto dynamicEventPtr = dynamic_cast<Event::DynamicEvent*>(event))
            {
                if (typeid(dynamicEventPtr) == typeid(Event::GamePlay::SetSpeed))
                    continue;
                if (dynamicEventPtr->angleOffset == 0)
                {
                    dynamicEventPtr->seconds = tiles[dynamicEventPtr->floor].seconds;
                    dynamicEventPtr->beat = tiles[dynamicEventPtr->floor].beat;
                }
                else
                {
                    const double bpm = getBpm(
                                     [&dynamicEventPtr](const Event::GamePlay::SetSpeed& ss)
                                     {
                                         return ss.floor < dynamicEventPtr->floor ||
                                             (ss.floor == dynamicEventPtr->floor &&
                                              ss.angleOffset <= dynamicEventPtr->angleOffset);
                                     }),
                                 spb = bpm2crotchet(bpm);
                    dynamicEventPtr->seconds =
                        tiles[dynamicEventPtr->floor].seconds + dynamicEventPtr->angleOffset / 180 * spb;
                    dynamicEventPtr->beat = seconds2beat(dynamicEventPtr->seconds);
                }

                dynamicEvents.push_back(dynamicEventPtr);
                m_processedDynamicEvents.push_back(dynamicEventPtr);
            }
            if (auto repeatEvents = dynamic_cast<Event::Modifiers::RepeatEvents*>(event))
            {
                vecRe[repeatEvents->floor].push_back(repeatEvents);
            }
        }

        // AnimateTrack
        for (size_t i = 0; i < tiles.size(); i++)
        {
            const double spb = bpm2crotchet(getBpmByBeat(tiles[tiles[i].trackAnimationFloor].beat)),
                         secondsAhead = tiles[i].beatsAhead * spb, secondsBehind = tiles[i].beatsBehind * spb;
            switch (tiles[i].trackAnimation)
            {
            case TrackAnimation::None:
                break;
            case TrackAnimation::Fade:
            default:
                const auto mtHide = new Event::Track::MoveTrack(), mtAppear = new Event::Track::MoveTrack();
                mtHide->floor = mtAppear->floor = i;
                mtHide->startTile = mtHide->endTile = mtAppear->startTile = mtAppear->endTile =
                    RelativeIndex(0, ThisTile);
                mtHide->beat = -settings.countdownTicks - 1;
                mtHide->seconds = mtHide->beat * bpm2crotchet(settings.bpm);
                mtHide->opacity = 0;
                mtAppear->seconds = tiles[i].seconds - secondsAhead;
                mtAppear->beat = seconds2beat(mtAppear->seconds);
                mtAppear->duration = 0.5;
                mtAppear->opacity = 100;
                mtHide->generated = mtAppear->generated = true;
                m_processedDynamicEvents.push_back(mtHide);
                m_processedDynamicEvents.push_back(mtAppear);
                break;
            }
            switch (tiles[i].trackDisappearAnimation)
            {
            case TrackDisappearAnimation::None:
                break;
            case TrackDisappearAnimation::Fade:
            default:
                if (i == tiles.size() - 1)
                    break;
                const auto mtDisappear = new Event::Track::MoveTrack();
                mtDisappear->floor = i;
                mtDisappear->startTile = mtDisappear->endTile = RelativeIndex(0, ThisTile);
                mtDisappear->seconds = tiles[i + 1].seconds - secondsBehind;
                mtDisappear->beat = seconds2beat(mtDisappear->seconds);
                mtDisappear->duration = 0.5;
                mtDisappear->opacity = 0;
                mtDisappear->generated = true;
                m_processedDynamicEvents.push_back(mtDisappear);
                break;
            }
        }

        // RepeatEvents
        for (const auto& event : dynamicEvents)
        {
            for (const auto& repeatEvents : vecRe[event->floor])
            {
                for (const auto& tag : repeatEvents->tag)
                {
                    for (const auto& eventTag : event->eventTag)
                    {
                        if (tag != eventTag)
                            continue;
                        const double spb = bpm2crotchet(getBpmByBeat(event->beat + event->angleOffset / 180));
                        if (repeatEvents->repeatType == Event::Modifiers::RepeatEvents::RepeatType::Beat)
                        {
                            const double gap = spb * repeatEvents->interval;
                            for (size_t i = 1; i <= repeatEvents->repetitions; i++)
                            {
                                auto eventClone = event->clone();
                                eventClone->seconds += gap * static_cast<double>(i);
                                eventClone->beat = seconds2beat(eventClone->seconds);
                                eventClone->generated = true;
                                m_processedDynamicEvents.push_back(eventClone);
                            }
                        }
                        else if (repeatEvents->repeatType == Event::Modifiers::RepeatEvents::RepeatType::Floor)
                        {
                            for (size_t i = 1; i <= repeatEvents->floorCount; i++)
                            {
                                const auto eventClone = event->clone();
                                eventClone->seconds =
                                    tiles[eventClone->floor + i].seconds + eventClone->angleOffset / 180 * spb;
                                eventClone->beat = seconds2beat(eventClone->seconds);
                                if (repeatEvents->executeOnCurrentFloor)
                                    eventClone->floor += i;
                                eventClone->generated = true;
                                m_processedDynamicEvents.push_back(eventClone);
                            }
                        }
                    }
                }
            }
        }

        std::ranges::stable_sort(m_processedDynamicEvents,
                                 [](const Event::DynamicEvent* a, const Event::DynamicEvent* b)
                                 { return a->beat < b->beat; });

        // MoveTrack
        for (const auto& event : m_processedDynamicEvents)
        {
            const auto mt = dynamic_cast<Event::Track::MoveTrack*>(event);
            if (mt == nullptr)
                continue;
            const size_t b = rel2absIndex(mt->floor, mt->startTile),
                         e = std::min(tiles.size() - 1, rel2absIndex(mt->floor, mt->endTile));
            for (size_t i = b; i <= e; i++)
            {
                auto& d = tiles[i].moveTrackDatas;
                d.emplace_back(mt->floor, mt->beat, mt->seconds, mt->startTile, mt->endTile, mt->duration,
                               mt->positionOffset, 114514, 114514, mt->rotationOffset, 114514, mt->scale, 114514,
                               114514, mt->opacity, 114514, mt->ease);
            }
        }
        for (auto& tile : tiles)
        {
            double xEndSec, yEndSec, rotEndSec, scXEndSec, scYEndSec,
                opEndSec = xEndSec = yEndSec = rotEndSec = scXEndSec = scYEndSec =
                    std::numeric_limits<double>::infinity();
            for (auto& moveTrackData : std::ranges::reverse_view(tile.moveTrackDatas))
            {
                moveTrackData.xEndSec = xEndSec;
                moveTrackData.yEndSec = yEndSec;
                moveTrackData.rotEndSec = rotEndSec;
                moveTrackData.scXEndSec = scXEndSec;
                moveTrackData.scYEndSec = scYEndSec;
                moveTrackData.opEndSec = opEndSec;
                if (moveTrackData.positionOffset.first)
                    xEndSec = moveTrackData.seconds;
                if (moveTrackData.positionOffset.second)
                    yEndSec = moveTrackData.seconds;
                if (moveTrackData.rotationOffset)
                    rotEndSec = moveTrackData.seconds;
                if (moveTrackData.scale.first)
                    scXEndSec = moveTrackData.seconds;
                if (moveTrackData.scale.second)
                    scYEndSec = moveTrackData.seconds;
                if (moveTrackData.opacity)
                    opEndSec = moveTrackData.seconds;
            }
        }

        tiles[0].beat = -std::numeric_limits<double>::infinity();
    }
    void Level::update()
    {
        if (!parsed)
            throw LevelNotParsedException();
        for (auto& tile : tiles)
        {
            tile.pos.o2c(), tile.scale.o2c(), tile.rotation.o2c(), tile.opacity = 100;
            tile.trackColor.o2c(), tile.secondaryTrackColor.o2c(), tile.trackStyle.o2c();
        }
    }
    void Level::update(const double& seconds)
    {
        if (!parsed)
            throw LevelNotParsedException();
        update();
        for (const auto& dynamicEvent : m_processedDynamicEvents)
        {
            if (seconds < dynamicEvent->seconds)
                break;
            if (const auto* const recolorTrack = dynamic_cast<Event::Track::RecolorTrack*>(dynamicEvent))
            {
                // double x, y;
                // if (!recolorTrack->duration || recolorTrack->duration == 0)
                //     x = y = 1;
                // else
                //     x = (seconds - recolorTrack->seconds) /
                //         (*recolorTrack->duration * bpm2crotchet(getBpmByBeat(recolorTrack->beat))),
                //     y = ease(recolorTrack->ease, x);
                const size_t b = rel2absIndex(recolorTrack->floor, recolorTrack->startTile),
                             e = std::min(tiles.size() - 1, rel2absIndex(recolorTrack->floor, recolorTrack->endTile));
                for (size_t i = b; i <= e; i++)
                {
                    tiles[i].trackColor.c = recolorTrack->trackColor;
                    tiles[i].secondaryTrackColor.c = recolorTrack->secondaryTrackColor;
                    tiles[i].trackStyle.c = recolorTrack->trackStyle;
                }
            }
        }
        for (auto& tile : tiles)
        {
            for (const auto& data : tile.moveTrackDatas)
            {
                const double bpm = getBpm([&data](const Event::GamePlay::SetSpeed& ss)
                                          { return data.beat >= ss.beat && data.floor >= ss.floor; }),
                             spb = bpm2crotchet(bpm);
                auto calcX = [&seconds, &data, &spb](const double endSec)
                {
                    return data.duration != 0.0 ? (std::min(seconds, endSec) - data.seconds) / spb / data.duration
                                                : 1.0;
                };
                if (data.positionOffset.first)
                {
                    const double x = calcX(data.xEndSec), y = ease(data.ease, x);
                    tile.pos.c.x += (tile.pos.o.x + *data.positionOffset.first - tile.pos.c.x) * y;
                }
                if (data.positionOffset.second)
                {
                    const double x = calcX(data.yEndSec), y = ease(data.ease, x);
                    tile.pos.c.y += (tile.pos.o.y + *data.positionOffset.second - tile.pos.c.y) * y;
                }
                if (data.rotationOffset)
                {
                    const double x = calcX(data.rotEndSec), y = ease(data.ease, x);
                    tile.rotation.c += (*data.rotationOffset - tile.rotation.c) * y;
                }
                if (data.scale.first)
                {
                    const double x = calcX(data.scXEndSec), y = ease(data.ease, x);
                    tile.scale.c.x += (*data.scale.first - tile.scale.c.x) * y;
                }
                if (data.scale.second)
                {
                    const double x = calcX(data.scYEndSec), y = ease(data.ease, x);
                    tile.scale.c.y += (*data.scale.second - tile.scale.c.y) * y;
                }
                if (data.opacity)
                {
                    const double x = calcX(data.opEndSec), y = ease(data.ease, x);
                    tile.opacity += (*data.opacity - tile.opacity) * y;
                }
            }
        }

        {
            // const double beat = seconds2beat(seconds);
            //
            // cameraPosition = { 0, 0 };
            // Vector2lf posOff = settings.position;
            // double x, y, relativeToPlayer = settings.relativeTo == "Player";
            // std::string relativeTo = settings.relativeTo;
            //
            // for (const auto& moveCamera : m_moveCameras)
            // {
            //     if (seconds < moveCamera->seconds) break;
            //     if (moveCamera->duration == 0)
            //         x = y = 1;
            //     else
            //         x = (seconds - moveCamera->seconds) / (moveCamera->duration *
            //         bpm2crotchet(getBpmByBeat(moveCamera->beat))), y = ease(moveCamera->ease, x);
            //
            //     if (moveCamera->relativeTo)
            //     {
            //         if (*moveCamera->relativeTo == "Tile" || *moveCamera->relativeTo == "Global")
            //         {
            //             auto& tile = tiles[*moveCamera->relativeTo == "Global" ? 0 : moveCamera->floor];
            //             relativeToPlayer += (0 - relativeToPlayer) * y;
            //             if (relativeTo == "Player")
            //                 cameraPosition = tile.pos.o;
            //             else
            //                 cameraPosition += (tile.pos.o - cameraPosition) * y;
            //         }
            //         else if (*moveCamera->relativeTo == "Player")
            //         {
            //             relativeToPlayer += (1 - relativeToPlayer) * y;
            //         }
            //         else if (*moveCamera->relativeTo == "LastPosition") // NEED TO OPTIMIZE
            //         {
            //             Vector2lf pos = cameraPosition;
            //             if (relativeTo == "Player")
            //                 pos = pos * (1 - relativeToPlayer)
            //                 + getCameraPosRelativeToPlayer(beat) * relativeToPlayer;
            //             cameraPosition = cameraPosition * (1 - y) + pos * y + posOff;
            //             relativeToPlayer = 0;
            //             posOff.x = posOff.y = 0;
            //         }
            //     }
            //
            //     if (moveCamera->position.first)
            //         posOff.x += (*moveCamera->position.first - posOff.x) * y;
            //     else if (moveCamera->relativeTo && *moveCamera->relativeTo != relativeTo)
            //         posOff.x += (0 - posOff.x) * y;
            //     if (moveCamera->position.second)
            //         posOff.y += (*moveCamera->position.second - posOff.y) * y;
            //     else if (moveCamera->relativeTo && *moveCamera->relativeTo != relativeTo)
            //         posOff.y += (0 - posOff.y) * y;
            //
            //     if (moveCamera->rotation)
            //         cameraRotation += (*moveCamera->rotation - cameraRotation) * y;
            //     if (moveCamera->zoom)
            //         cameraZoom += (*moveCamera->zoom - cameraZoom) * y;
            //
            //     if (moveCamera->relativeTo) relativeTo = *moveCamera->relativeTo;
            // }
            // if (relativeToPlayer != 0)
            // {
            //     Vector2lf playerPos = getCameraPosRelativeToPlayer(beat);
            //     cameraPosition = cameraPosition * (1 - relativeToPlayer) + playerPos * relativeToPlayer;
            // }
            // cameraPosition += posOff;
        }
    }
    size_t Level::rel2absIndex(const size_t& baseIndex, const RelativeIndex& relativeIndex) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        const bool positive = relativeIndex.index > 0ll;
        const size_t maxIdx = tiles.size() - 1ull, absRelIdx = static_cast<size_t>(std::abs(relativeIndex.index));
        switch (relativeIndex.relativeTo)
        {
        case Start:
            return positive ? std::min(absRelIdx, maxIdx) : 0ull;
        case ThisTile:
            return positive             ? std::min(baseIndex + absRelIdx, maxIdx)
                : baseIndex > absRelIdx ? baseIndex - absRelIdx
                                        : 0ull;
        case End:
            return positive ? maxIdx : maxIdx > absRelIdx ? maxIdx - absRelIdx : 0ull;
        default:
            unreachable();
        }
    }
    double Level::getPlanetsDir(const size_t& index, const double& seconds) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        const double bpm = getBpm([&index, &seconds](const Event::GamePlay::SetSpeed& ss)
                                  { return ss.floor <= index && ss.seconds <= seconds; }),
                     spb = bpm2crotchet(bpm);
        double angle;
        if (index == 0)
        {
            angle = -seconds / spb * 180;
        }
        else
        {
            if (tiles[index].angle.deg() == 999)
                angle = tiles[index - 1].angle.deg();
            else
                angle = tiles[index].angle.deg() - 180;
            if (tiles[index].orbit == Clockwise)
                angle -= ((seconds - tiles[index].seconds) / spb) * 180;
            else
                angle += ((seconds - tiles[index].seconds) / spb) * 180;
        }
        return angle;
    }
    std::pair<Vector2lf, Vector2lf> Level::getPlanetsPos(const size_t& index, const double& seconds) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        Vector2lf p2, p1 = p2 = tiles[index].stickToFloors ? tiles[index].pos.c : tiles[index].pos.o;
        const double angle = getPlanetsDir(index, seconds);
        p2.x += cos(deg2rad(angle)), p2.y += sin(deg2rad(angle));
        if (isFirePlanetStatic(index))
            return std::make_pair(p1, p2);
        return std::make_pair(p2, p1);
    }
    inline bool Level::isFirePlanetStatic(const size_t index) { return index % 2 == 0; }
    size_t Level::getTileIndexByBeat(const double& beat) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        return std::upper_bound(tiles.begin() + 1, tiles.end(), beat,
                                [](const double& val, const Tile& e) -> bool { return val < e.beat; }) -
            (tiles.begin() + 1);
    }
    size_t Level::getTileIndexBySeconds(const double& seconds) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        return std::upper_bound(tiles.begin() + 1, tiles.end(), seconds,
                                [](const double& val, const Tile& e) -> bool { return val < e.seconds; }) -
            (tiles.begin() + 1);
    }
    double Level::getBpm(const std::function<bool(const Event::GamePlay::SetSpeed&)>& func) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        double bpm = settings.bpm;
        for (const auto& setSpeed : m_setSpeeds)
        {
            if (!func(*setSpeed))
                break;
            if (setSpeed->speedType == Event::GamePlay::SetSpeed::SpeedType::Bpm)
                bpm = setSpeed->beatsPerMinute;
            else
                bpm *= setSpeed->bpmMultiplier;
        }
        return bpm;
    }
    double Level::getBpmByBeat(const double& beat) const
    {
        return getBpm([&](const Event::GamePlay::SetSpeed& ss) { return beat >= ss.beat; });
    }
    double Level::getBpmBySeconds(const double& seconds) const
    {
        return getBpm([&](const Event::GamePlay::SetSpeed& ss) { return seconds >= ss.seconds; });
    }
    double Level::getBpmExcludingBeat(const double& beat) const
    {
        return getBpm([&](const Event::GamePlay::SetSpeed& ss) { return beat > ss.beat; });
    }
    double Level::beat2seconds(const double& beat) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        double bpm = settings.bpm, b = 0, seconds = settings.offset / 1000;
        for (const auto& setSpeed : m_setSpeeds)
        {
            seconds += bpm2crotchet(bpm) * (std::min(beat, setSpeed->beat) - b);
            b = setSpeed->beat;
            if (setSpeed->speedType == Event::GamePlay::SetSpeed::SpeedType::Bpm)
                bpm = setSpeed->beatsPerMinute;
            else
                bpm *= setSpeed->bpmMultiplier;
            if (beat <= setSpeed->beat)
                return seconds;
        }
        seconds += bpm2crotchet(bpm) * (beat - b);
        return seconds;
    }

    double Level::seconds2beat(double seconds) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        seconds -= settings.offset / 1000;
        double bpm = settings.bpm, last_beat = 0, beat = 0;
        for (const auto& setSpeed : m_setSpeeds)
        {
            if (seconds > bpm2crotchet(bpm) * (setSpeed->beat - last_beat))
                seconds -= bpm2crotchet(bpm) * (setSpeed->beat - last_beat);
            else
            {
                beat += seconds / bpm2crotchet(bpm);
                return beat;
            }
            beat += setSpeed->beat - last_beat;
            if (setSpeed->speedType == Event::GamePlay::SetSpeed::SpeedType::Bpm)
                bpm = setSpeed->beatsPerMinute;
            else
                bpm *= setSpeed->bpmMultiplier;
            last_beat = setSpeed->beat;
        }
        if (seconds != 0)
            beat += seconds / bpm2crotchet(bpm);
        return beat;
    }

    double Level::getAngle(const size_t& i) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        if (tiles[i].angle.deg() == 999)
            return 0;
        double angle;
        if (tiles[i - 1].angle.deg() == 999)
            angle = tiles[i - 2].angle.deg() - tiles[i].angle.deg();
        else
            angle = tiles[i - 1].angle.deg() - 180 - tiles[i].angle.deg();
        if (tiles[i - 1].orbit == CounterClockwise)
            angle *= -1;
        while (angle <= 0)
            angle += 360;
        while (angle > 360)
            angle -= 360;
        return angle;
    }

    // Vector2lf Level::getCameraPosRelativeToPlayer(const double& beat) const
    // {
    //     if (!parsed) throw LevelNotParsedException();
    //     double a; Vector2lf pos;
    //     for (size_t j = 1; j < tiles.size(); j++)
    //     {
    //         if (beat < tiles[j].beat) break;
    //         if (j != tiles.size() - 1 && tiles[j + 1].angle == 999) continue;
    //         if (j == tiles.size() - 1)
    //             a = beat - tiles[j].beat;
    //         else
    //             a = std::min(beat, tiles[j + 1].beat) - tiles[j].beat;
    //         a = std::min(a / 2, 1.0);
    //         pos += (tiles[j].pos.o - pos) * a;
    //     }
    //     return pos;
    // }

    // void Level::calcCameraPlayer(const double& seconds, const size_t& floor)
    // {
    //     if (!parsed)
    //         throw LevelNotParsedException();
    //     const double delta = seconds - m_camera.lastSeconds;
    //     if (!std::isnormal(delta))
    //         throw std::exception();
    //     const Vector2lf& targetPos = tiles[floor].pos.o;
    //     if (floor != m_camera.lastFloor)
    //         m_camera.lastFloor = floor, m_camera.lastChangedPos = m_camera.player;
    //     const double gapDis = (targetPos - m_camera.lastChangedPos).length(),
    //                  speed = gapDis * getBpmBySeconds(seconds) / 60.0 / 2.0;
    //     const Vector2lf v = targetPos - m_camera.player, n = v.normalized() * delta * speed;
    //     if ((m_camera.player - targetPos).lengthSquared() > n.lengthSquared())
    //         m_camera.player += n;
    //     else
    //         m_camera.player = targetPos;
    // }

    void Level::initCamera()
    {
        if (!parsed)
            throw LevelNotParsedException();
        m_camera = Camera();
        m_moveCameraDatas.clear();
        double rotEndSec, zoomEndSec, xEndSec, yEndSec,
            relEndSec = xEndSec = yEndSec = rotEndSec = zoomEndSec = std::numeric_limits<double>::infinity();
        m_moveCameraDatas.emplace_back(0, -settings.countdownTicks,
                                       -settings.countdownTicks * bpm2crotchet(settings.bpm), 0.0, settings.relativeTo,
                                       false, relEndSec, OptionalPoint(), xEndSec, yEndSec, settings.rotation,
                                       rotEndSec, settings.zoom, zoomEndSec, Easing::Linear);
        for (const auto& m_processedDynamicEvent : std::ranges::reverse_view(m_processedDynamicEvents))
        {
            const auto mc = dynamic_cast<Event::Visual::MoveCamera*>(m_processedDynamicEvent);
            if (mc == nullptr)
                continue;
            m_moveCameraDatas.emplace(m_moveCameraDatas.begin() + 1, mc->floor, mc->beat, mc->seconds, mc->duration,
                                      mc->relativeTo, false, 114514, mc->position, xEndSec, yEndSec, mc->rotation,
                                      rotEndSec, mc->zoom, zoomEndSec, mc->ease);
            if (mc->relativeTo)
                relEndSec = mc->seconds;
            if (mc->position.first)
                xEndSec = mc->seconds;
            if (mc->position.second)
                yEndSec = mc->seconds;
            if (mc->rotation)
                rotEndSec = mc->seconds;
            if (mc->zoom)
                zoomEndSec = mc->seconds;
        }
        RelativeToCamera lastRel = settings.relativeTo;
        for (auto it = m_moveCameraDatas.begin() + 1; it != m_moveCameraDatas.end(); ++it)
        {
            it->duplicatedRelPlayer =
                lastRel == RelativeToCamera::Player && (!it->relativeTo.has_value() || *it->relativeTo == lastRel);
            if (!it->duplicatedRelPlayer)
                lastRel = *it->relativeTo;
        }
        for (auto& m_moveCameraData : std::ranges::reverse_view(m_moveCameraDatas))
        {
            m_moveCameraData.relEndSec = relEndSec;
            if (!m_moveCameraData.duplicatedRelPlayer)
                relEndSec = m_moveCameraData.seconds;
        }
    }

    void Level::updateCamera(const double& seconds, const size_t& floor)
    {
        if (!parsed)
            throw LevelNotParsedException();
        Vector2lf pos, posOff;
        double rot{}, zoom{};
        for (const auto& data : m_moveCameraDatas)
        {
            const double bpm = getBpm([&data](const Event::GamePlay::SetSpeed& ss)
                                      { return data.beat >= ss.beat && data.floor >= ss.floor; }),
                         spb = bpm2crotchet(bpm);
            if (seconds < data.seconds)
                break;

            auto calcX = [&seconds, &data, &spb](const double endSec)
            { return data.duration != 0.0 ? (std::min(seconds, endSec) - data.seconds) / spb / data.duration : 1.0; };

            if (data.relativeTo && !data.duplicatedRelPlayer)
            {
                const double x = calcX(data.relEndSec), y = ease(data.ease, x);
                switch (*data.relativeTo)
                {
                case RelativeToCamera::Player:
                    {
                        if (seconds > data.relEndSec)
                            break;

                        const double delta = std::isinf(m_camera.lastSeconds) ? 0 : seconds - m_camera.lastSeconds;
                        if (!std::isnormal(delta) && delta != 0)
                            throw std::logic_error(
                                "Delta (seconds - m_camera.lastSeconds) is not normal and is not zero");
                        const Vector2lf& targetPos = tiles[floor].pos.o;
                        if (floor != m_camera.lastFloor)
                            m_camera.lastFloor = floor, m_camera.lastChangedPos = m_camera.player;
                        const double gapDis = (targetPos - m_camera.lastChangedPos).length(),
                                     speed = gapDis * getBpmBySeconds(seconds) / 60.0 / 2.0;
                        if (targetPos != m_camera.player)
                        {
                            const Vector2lf v = targetPos - m_camera.player, n = v.normalized() * delta * speed;
                            if ((m_camera.player - targetPos).lengthSquared() > n.lengthSquared())
                                m_camera.player += n;
                            else
                                m_camera.player = targetPos;
                        }

                        pos += (m_camera.player - pos) * y;
                        break;
                    }
                case RelativeToCamera::Tile:
                    pos += (tiles[data.floor].pos.o - pos) * y;
                    if (seconds <= data.relEndSec)
                        m_camera.player = pos;
                    break;
                case RelativeToCamera::Global:
                    // pos += (Vector2lf(0, 0) - pos) * y;
                    pos += -pos * y;
                    if (seconds <= data.relEndSec)
                        m_camera.player = pos;
                    break;
                case RelativeToCamera::LastPosition:
                    pos += posOff;
                    posOff = Vector2lf(0, 0);
                    if (seconds <= data.relEndSec)
                        m_camera.player = pos;
                    break;
                default:
                    unreachable();
                }
            }

            if (data.position.first)
            {
                const double x = calcX(data.xEndSec), y = ease(data.ease, x);
                posOff.x += (*data.position.first - posOff.x) * y;
            }
            if (data.position.second)
            {
                const double x = calcX(data.yEndSec), y = ease(data.ease, x);
                posOff.y += (*data.position.second - posOff.y) * y;
            }
            if (data.rotation)
            {
                const double x = calcX(data.rotEndSec), y = ease(data.ease, x);
                rot += (*data.rotation - rot) * y;
            }
            if (data.zoom)
            {
                const double x = calcX(data.zoomEndSec), y = ease(data.ease, x);
                zoom += (*data.zoom - zoom) * y;
            }
        }
        m_camera.position = pos + posOff;
        m_camera.rotation = rot;
        m_camera.zoom = zoom;
        m_camera.lastSeconds = seconds;
    }

    double Level::getTiming(const size_t& index, const double& seconds) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        return seconds - tiles[index].seconds;
    }

    Level::TimingBoundary Level::getTimingBoundary(const size_t& index, const Difficulty& difficulty) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        double bpm = settings.bpm, k = 1;
        for (const auto& setSpeed : m_setSpeeds)
        {
            if (tiles[index].beat <= setSpeed->beat)
                break;
            if (setSpeed->speedType == Event::GamePlay::SetSpeed::SpeedType::Bpm)
                bpm = setSpeed->beatsPerMinute, k = 1;
            else
                bpm *= setSpeed->bpmMultiplier, k *= setSpeed->bpmMultiplier;
        }
        k = 1;
        const double seconds = std::max(bpm2crotchet(bpm),
                                        difficulty == Difficulty::Lenient      ? 91.0 * 3 / 1000
                                            : difficulty == Difficulty::Normal ? 65.0 * 3 / 1000
                                                                               : 40.0 * 3 / 1000),
                     p = std::max(25.0 / 1000, seconds / 6 / k), lep = std::max(25.0 / 1000, seconds / 4 / k),
                     vle = std::max(25.0 / 1000, seconds / 3 / k);
        return {p, lep, vle};
    }

    HitMargin Level::getHitMargin(const size_t& index, const double& seconds, const Difficulty& difficulty) const
    {
        if (!parsed)
            throw LevelNotParsedException();
        const auto [p, lep, vle] = getTimingBoundary(index, difficulty);
        const double timing = getTiming(index, seconds);
        if (timing > vle)
            return HitMargin::TooLate;
        if (timing > lep)
            return HitMargin::VeryLate;
        if (timing > p)
            return HitMargin::LatePerfect;
        if (timing > -p)
            return HitMargin::Perfect;
        if (timing > -lep)
            return HitMargin::EarlyPerfect;
        if (timing > -vle)
            return HitMargin::VeryEarly;
        return HitMargin::TooEarly;
    }
    bool Level::isParsed() const { return parsed; }

    Level::CameraValue Level::cameraValue() const
    {
        if (!parsed)
            throw LevelNotParsedException();
        return {m_camera.position, m_camera.rotation, m_camera.zoom};
    }
} // namespace AdoCpp
