#include "Level.h"

#include <algorithm>
#include <optional>
#include <rapidjson/error/en.h>
#include <filesystem>
#include "Utils.h"

namespace AdoCpp
{
    std::map<char, double> path2angle =
    {
        {'R', 0}, {'p', 15}, {'J', 30}, {'E', 45}, {'T', 60}, {'o', 75},
        {'U', 90}, {'q', 105}, {'G', 120}, {'Q', 135}, {'H', 150}, {'W', 165},
        {'L', 180}, {'x', 195}, {'N', 210}, {'Z', 225}, {'F', 240}, {'V', 255},
        {'D', 270}, {'Y', 285}, {'B', 300}, {'C', 315}, {'M', 330}, {'A', 345},
        {'5', 555}, {'6', 666}, {'7', 777}, {'8', 888}, {'!', 999},
    };

    Level::Level()
    {
    }

    Level::Level(const rapidjson::Document& data)
    {
        fromJson(data);
    }

    Level::Level(std::ifstream& ifs)
    {
        fromFile(ifs);
    }

    Level::Level(const std::filesystem::path& path)
    {
        fromFile(path);
    }

    Level::~Level()
    {
        clear();
    }

    static inline double deg2rad(double deg)
    {
        return deg * 3.141592653589793 / 180;
    }

    void Level::clear()
    {
        settings = Settings();
        tiles.clear();
        while (!m_processedBeatEvents.empty())
        {
            if (m_processedBeatEvents.back()->repeat)
                delete m_processedBeatEvents.back();
            m_processedBeatEvents.pop_back();
        }
        while (!events.empty())
        {
            delete events.back();
            events.pop_back();
        }
        m_moveCameraPlayerPosVec.clear();
        m_moveCameraPlayerPosVec2.clear();
        m_moveCameras.clear();
        m_setSpeeds.clear();
        parsed = false;
    }

    void Level::defaultLevel()
    {
        clear();
        tiles.push_back(Tile(0));
        tiles[0].beat = -INFINITY;
        for (size_t i = 0; i < 10; i++)
            tiles.push_back(Tile(0));
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
        settings.trackColorType = string2trackColorType[s["trackColorType"].GetString()];
        settings.trackColor = s["trackColor"].GetString();
        settings.secondaryTrackColor = s["secondaryTrackColor"].GetString();
        settings.trackColorAnimDuration = s["trackColorAnimDuration"].GetDouble();
        settings.trackColorPulse = string2trackColorPulse[s["trackColorPulse"].GetString()];
        settings.trackPulseLength = s["trackPulseLength"].GetDouble();
        settings.trackStyle = string2trackStyle[s["trackStyle"].GetString()];
        //settings.trackAnimation = string2trackAnimation[s["trackAnimation"].GetString()];
        settings.beatsAhead = s["beatsAhead"].GetDouble();
        //settings.trackDisappearAnimation = string2trackAnimation[s["trackDisappearAnimation"].GetString()];
        settings.beatsBehind = s["beatsBehind"].GetDouble();
        settings.backgroundColor = s["backgroundColor"].GetString();
        settings.stickToFloors = toBool(s["stickToFloors"]);
        settings.unscaledSize = s["unscaledSize"].GetDouble();
        settings.relativeTo = s["relativeTo"].GetString();
        settings.position = Point(s["position"][0].GetDouble(), s["position"][1].GetDouble());
        settings.rotation = s["rotation"].GetDouble();
        settings.zoom = s["zoom"].GetDouble();

        bool angleData = document.HasMember("angleData");
        tiles.push_back(Tile(0));
        tiles[0].beat = -INFINITY;
        if (angleData)
        {
            for (auto &angle : document["angleData"].GetArray())
                tiles.push_back(Tile(angle.GetDouble()));
        }
        else
        {
            if (!document.HasMember("pathData"))
                throw std::exception();
            for (auto &path : std::string(document["pathData"].GetString()))
                tiles.push_back(Tile(path2angle[path]));
        }

        for (auto &eventData : document["actions"].GetArray())
        {
            size_t f = eventData["floor"].GetUint64();
            std::string eventType = eventData["eventType"].GetString();
            if (eventType == "SetSpeed")
                events.push_back(new Event::GamePlay::SetSpeed(eventData));
            else if (eventType == "Twirl")
                events.push_back(new Event::GamePlay::Twirl(eventData));
            else if (eventType == "Pause")
                events.push_back(new Event::GamePlay::Pause(eventData));

            else if (eventType == "ColorTrack")
                events.push_back(new Event::Track::ColorTrack(eventData));
            else if (eventType == "RecolorTrack")
                events.push_back(new Event::Track::RecolorTrack(eventData));
            else if (eventType == "PositionTrack")
                events.push_back(new Event::Track::PositionTrack(eventData));
            else if (eventType == "MoveTrack")
                events.push_back(new Event::Track::MoveTrack(eventData));

            else if (eventType == "MoveCamera")
                events.push_back(new Event::Visual::MoveCamera(eventData));

            else if (eventType == "RepeatEvents")
                events.push_back(new Event::Modifiers::RepeatEvents(eventData));
        }
    }

    void Level::fromFile(std::ifstream& ifs)
    {
        parsed = false;
        rapidjson::Document document;
        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::AutoUTFInputStream<unsigned, rapidjson::IStreamWrapper> eis(isw);
        document.ParseStream<
            rapidjson::kParseValidateEncodingFlag
            | rapidjson::kParseCommentsFlag
            | rapidjson::kParseTrailingCommasFlag
            | rapidjson::kParseNanAndInfFlag
            | rapidjson::kParseFullPrecisionFlag
            ,
            rapidjson::AutoUTF<unsigned>
        >(eis);
        if (document.HasParseError())
        {
            //document.GetParseError();
            std::cout << rapidjson::GetParseError_En(document.GetParseError());
            throw LevelJsonHasParseErrorException();
        }
        fromJson(document);
    }
    void Level::fromFile(const std::filesystem::path& path)
    {
        parsed = false;
        std::ifstream ifs(path);
        if (!ifs.is_open()) throw LevelCouldNotOpenFileException();
        fromFile(ifs);
        ifs.close();
    }

    void Level::parse()
    {
        if (parsed) return;
        if (tiles.size() <= 1)
        {
            parsed = false;
            return;
        }
        std::vector<bool> twirls(tiles.size());
        std::vector<double> pauses(tiles.size());
        std::vector<std::optional<Event::Track::PositionTrack> > positionTracks(tiles.size());
        std::vector<std::optional<Event::Track::ColorTrack> > colorTracks(tiles.size());
        for (auto& event : events)
        {
            if (typeid(*event) == typeid(Event::GamePlay::Twirl))
                twirls[event->floor] = true;
            else if (auto pause = dynamic_cast<Event::GamePlay::Pause*>(event))
                pauses[pause->floor] = pause->duration;

            else if (auto positionTrack = dynamic_cast<Event::Track::PositionTrack*>(event))
                positionTracks[positionTrack->floor] = (std::make_optional<Event::Track::PositionTrack>(*positionTrack));
            else if (auto colorTrack = dynamic_cast<Event::Track::ColorTrack*>(event))
                colorTracks[colorTrack->floor] = (std::make_optional<Event::Track::ColorTrack>(*colorTrack));
        }
        tiles[0].orbit = true,
            tiles[0].beat = 0,
            tiles[0].oTrackColor = settings.trackColor,
            tiles[0].oSecondaryTrackColor = settings.secondaryTrackColor,
            tiles[0].oTrackStyle = settings.trackStyle,
            tiles[0].editorPos = tiles[0].oPos = {0, 0},
            tiles[0].stickToFloors = settings.stickToFloors;
        for (size_t i = 1; i < tiles.size(); i++)
        {
            // Tile's twirl
            tiles[i].orbit = tiles[i - 1].orbit;
            if (twirls[i])
                tiles[i].orbit = !tiles[i].orbit;

            // Tile's beat
            if (tiles[i].angle == 999)
            {
                tiles[i].beat = tiles[i - 1].beat;
            }
            else
            {
                double angle, beat;
                if (tiles[i - 1].angle == 999)
                    angle = tiles[i - 2].angle - tiles[i].angle;
                else
                    angle = tiles[i - 1].angle - 180 - tiles[i].angle;
                if (!tiles[i - 1].orbit)
                    angle *= -1;
                while (angle <= 0) angle += 360;
                while (angle > 360) angle -= 360;
                if (i == 1)
                    angle -= 180;
                beat = angle / 180 + pauses[i - 1],
                    tiles[i].beat = tiles[i - 1].beat + beat;
            }

            // Tile's position
            tiles[i].stickToFloors = tiles[i - 1].stickToFloors;
            tiles[i].oPos.first += tiles[i - 1].oPos.first,
                tiles[i].oPos.second += tiles[i - 1].oPos.second,
                tiles[i].editorPos.first = tiles[i - 1].editorPos.first,
                tiles[i].editorPos.second = tiles[i - 1].editorPos.second;
            if ((i + 1 == tiles.size() || tiles[i + 1].angle != 999) && tiles[i].angle != 999)
                tiles[i].oPos.first += cos(deg2rad(tiles[i].angle)),
                tiles[i].editorPos.first += cos(deg2rad(tiles[i].angle)),
                tiles[i].oPos.second += sin(deg2rad(tiles[i].angle)),
                tiles[i].editorPos.second += sin(deg2rad(tiles[i].angle));
            if (positionTracks[i])
            {
                tiles[i].editorPos.first += positionTracks[i]->positionOffset.first,
                    tiles[i].editorPos.second += positionTracks[i]->positionOffset.second;
                if (positionTracks[i]->justThisTile && i != tiles.size() - 1)
                {
                    tiles[i + 1].oPos.first -= positionTracks[i]->positionOffset.first,
                        tiles[i + 1].oPos.second -= positionTracks[i]->positionOffset.second;
                }
                if (!positionTracks[i]->editorOnly)
                {
                    tiles[i].oPos.first += positionTracks[i]->positionOffset.first,
                        tiles[i].oPos.second += positionTracks[i]->positionOffset.second;
                }
                if (positionTracks[i]->stickToFloors)
                    tiles[i].stickToFloors = *positionTracks[i]->stickToFloors;
            }

            // Tile's color
            tiles[i].oTrackColor = tiles[i - 1].oTrackColor,
                tiles[i].oSecondaryTrackColor = tiles[i - 1].oSecondaryTrackColor,
                tiles[i].oTrackStyle = tiles[i - 1].oTrackStyle;
            if (colorTracks[i])
            {
                tiles[i].oTrackColor = colorTracks[i]->trackColor,
                    tiles[i].oSecondaryTrackColor = colorTracks[i]->secondaryTrackColor,
                    tiles[i].oTrackStyle = colorTracks[i]->trackStyle;
            }
        }
        tiles[0].beat = -INFINITY;
        std::vector<AdoCpp::Event::BeatEvent*> beatEvents;
        m_processedBeatEvents.clear();
        std::vector<std::vector<Event::Modifiers::RepeatEvents>> vecRe{ tiles.size() };
        for (auto& event : events)
        {
            tiles[event->floor].events.push_back(event);
            if (auto beatEventPtr = dynamic_cast<Event::BeatEvent*>(event))
            {
                beatEventPtr->beat = tiles[beatEventPtr->floor].beat + beatEventPtr->angleOffset / 180;
                beatEvents.push_back(beatEventPtr);
                m_processedBeatEvents.push_back(beatEventPtr);
            }
            if (auto repeatEvents = dynamic_cast<Event::Modifiers::RepeatEvents*>(event))
            {
                vecRe[repeatEvents->floor].push_back(*repeatEvents);
            }
        }
        
        for (auto& event : beatEvents)
        {
            for (auto &repeatEvents : vecRe[event->floor])
            {
                for (auto& tag : repeatEvents.tag)
                {
                    for (auto& eventTag : event->eventTag)
                    {
                        if (tag != eventTag) continue;
                        if (repeatEvents.repeatType ==
                            Event::Modifiers::RepeatEvents::RepeatType::Beat)
                        {
                            for (size_t i = 1; i <= repeatEvents.repetitions; i++)
                            {
                                auto eventClone = event->clone();
                                eventClone->beat += repeatEvents.interval * i;
                                eventClone->repeat = true;
                                m_processedBeatEvents.push_back(eventClone);
                            }
                        }
                        else if (repeatEvents.repeatType == 
                            Event::Modifiers::RepeatEvents::RepeatType::Floor)
                        {
                            for (size_t i = 1; i <= repeatEvents.floorCount; i++)
                            {
                                auto eventClone = event->clone();
                                if (repeatEvents.executeOnCurrentFloor)
                                    eventClone->floor += i;
                                eventClone->beat
                                    = tiles[eventClone->floor + i].beat
                                    + eventClone->angleOffset / 180;
                                eventClone->repeat = true;
                                m_processedBeatEvents.push_back(eventClone);
                            }
                        }
                    }
                }
            }
        }
       
        std::stable_sort(
            m_processedBeatEvents.begin(), m_processedBeatEvents.end(),
            [](const Event::BeatEvent* a, const Event::BeatEvent* b)
            //{ return a->beat != b->beat ? a->beat < b->beat : a->floor < b->floor; }
            { return a->beat < b->beat; }
        );
        m_setSpeeds.clear();
        m_moveCameras.clear();
        m_moveCameraPlayerPosVec.clear();
        m_moveCameraPlayerPosVec2.clear();
        for (auto& event : m_processedBeatEvents)
        {
            if (auto setSpeed = dynamic_cast<Event::GamePlay::SetSpeed*>(event))
            {
                m_setSpeeds.push_back(*setSpeed);
            }
            if (auto moveCamera = dynamic_cast<Event::Visual::MoveCamera*>(event))
            {
                m_moveCameras.push_back(*moveCamera);
            }
        }

        parsed = true;

        std::string relativeTo = "Player";

        for (auto& moveCamera : m_moveCameras)
        {
            if (moveCamera.relativeTo)
            {
                if (relativeTo == "Tile" && *moveCamera.relativeTo == "Player")
                    m_moveCameraPlayerPosVec.push_back(f(moveCamera.beat + moveCamera.duration));
                else
                    m_moveCameraPlayerPosVec.push_back(Point(114514, 1919810));
                if (relativeTo == "Player" && *moveCamera.relativeTo == "Tile")
                    m_moveCameraPlayerPosVec2.push_back(f(moveCamera.beat));
                else
                    m_moveCameraPlayerPosVec2.push_back(Point(114514, 1919810));
                relativeTo = *moveCamera.relativeTo;
            }
            else
            {
                m_moveCameraPlayerPosVec.push_back(Point(114514, 1919810));
                m_moveCameraPlayerPosVec2.push_back(Point(114514, 1919810));
            }
        }
    }
    void Level::update()
    {
        if (!parsed) throw LevelNotParsedException();
        for (auto& tile : tiles)
        {
            tile.pos = tile.oPos,
                tile.scale = tile.oScale,
                tile.rotation = tile.oRotation,
                tile.opacity = 100;
            tile.trackColor = tile.oTrackColor,
                tile.secondaryTrackColor = tile.oSecondaryTrackColor,
                tile.trackStyle = tile.oTrackStyle;
        }
    }
    void Level::update(const double& beat)
    {
        if (!parsed) throw LevelNotParsedException();
        update();
        for (auto& event_ : m_processedBeatEvents)
        {
            if (beat < event_->beat) break;
            if (auto moveTrack = dynamic_cast<Event::Track::MoveTrack*>(event_))
            {
                double x, y;
                if (moveTrack->duration == 0)
                    x = y = 1;
                else
                    x = (beat - moveTrack->beat) / moveTrack->duration,
                    y = ease(moveTrack->ease, x);
                size_t b = rel2absIndex(moveTrack->floor, moveTrack->startTile),
                    e = rel2absIndex(moveTrack->floor, moveTrack->endTile);
                for (size_t i = b; i <= e && i < tiles.size(); i++)
                {
                    if (moveTrack->positionOffset.first)
                        tiles[i].pos.first += (tiles[i].oPos.first + *moveTrack->positionOffset.first - tiles[i].pos.first) * y;
                    if (moveTrack->positionOffset.second)
                        tiles[i].pos.second += (tiles[i].oPos.second + *moveTrack->positionOffset.second - tiles[i].pos.second) * y;
                    if (moveTrack->rotationOffset)
                        tiles[i].rotation += (*moveTrack->rotationOffset - tiles[i].rotation) * y;
                    if (moveTrack->scale.first)
                        tiles[i].scale.first += (*moveTrack->scale.first - tiles[i].scale.first) * y;
                    if (moveTrack->scale.second)
                        tiles[i].scale.second += (*moveTrack->scale.second - tiles[i].scale.second) * y;
                    if (moveTrack->opacity)
                        tiles[i].opacity += (*moveTrack->opacity - tiles[i].opacity) * y;
                }
            }
            else if (auto recolorTrack = dynamic_cast<Event::Track::RecolorTrack*>(event_))
            {
                double x, y;
                if (!recolorTrack->duration
                    || recolorTrack->duration == 0)
                    x = y = 1;
                else
                    x = (beat - recolorTrack->beat) / *recolorTrack->duration,
                    y = ease(recolorTrack->ease, x);
                size_t b = rel2absIndex(recolorTrack->floor, recolorTrack->startTile),
                    e = rel2absIndex(recolorTrack->floor, recolorTrack->endTile);
                for (size_t i = b; i <= e && i < tiles.size(); i++)
                {
                    tiles[i].trackColor = recolorTrack->trackColor;
                    tiles[i].secondaryTrackColor = recolorTrack->secondaryTrackColor;
                    tiles[i].trackStyle = recolorTrack->trackStyle;
                }
            }
        }
    }
    size_t Level::rel2absIndex(const size_t& index, const RelativeIndex& relativeIndex) const
    {
        if (!relativeIndex.second.compare("Start"))
            return relativeIndex.first;
        else if (!relativeIndex.second.compare("End"))
            return tiles.size() - 1 + relativeIndex.first;
        else if (!relativeIndex.second.compare("ThisTile"))
            return index + relativeIndex.first;
        throw std::exception();
    }

    std::pair<Point, Point> Level::getPlanetsPos(const size_t& index, const double& beat) const
    {
        if (!parsed) throw LevelNotParsedException();
        Point p1, p2;
        double angle;
        if (tiles[index].stickToFloors)
            p1 = tiles[index].pos;
        else
            p1 = tiles[index].oPos;
        if (index == 0)
            angle = -beat * 180;
        else
        {
            if (tiles[index].angle == 999)
                angle = tiles[index - 1].angle;
            else
                angle = tiles[index].angle - 180;
            if (tiles[index].orbit)
                angle -= (beat - tiles[index].beat) * 180;
            else
                angle += (beat - tiles[index].beat) * 180;
        }

        p2 = p1;
        p2.first += cos(deg2rad(angle)),
            p2.second += sin(deg2rad(angle));
        if (isFirePlanetStatic(index))
            return std::make_pair(p1, p2);
        else
            return std::make_pair(p2, p1);
        return std::pair<Point, Point>();
    }
    inline bool Level::isFirePlanetStatic(size_t index) const
    {
        return index % 2 == 0;
    }
    size_t Level::getTileIndexByBeat(const double& beat) const
    {
        if (!parsed) throw LevelNotParsedException();
        return std::upper_bound(
            tiles.begin() + 1, tiles.end(), beat,
            [](const double& val, Tile e) -> bool {return val < e.beat;}
        ) - (tiles.begin() + 1);
    }
    double Level::getBpmByBeat(const double& beat) const
    {
        if (!parsed) throw LevelNotParsedException();
        double bpm = settings.bpm;
        for (auto& setSpeed : m_setSpeeds)
        {
            if (beat < setSpeed.beat)
                break;
            if (setSpeed.speedType == Event::GamePlay::SetSpeed::SpeedType::Bpm)
                bpm = setSpeed.beatsPerMinute;
            else
                bpm *= setSpeed.bpmMultiplier;
        }
        return bpm;
    }
    double Level::getBpmNotIncludingBeat(const double& beat) const
    {
        if (!parsed) throw LevelNotParsedException();
        double bpm = settings.bpm;
        for (auto& setSpeed : m_setSpeeds)
        {
            if (beat <= setSpeed.beat)
                break;
            if (setSpeed.speedType == Event::GamePlay::SetSpeed::SpeedType::Bpm)
                bpm = setSpeed.beatsPerMinute;
            else
                bpm *= setSpeed.bpmMultiplier;
        }
        return bpm;
    }
    double Level::beat2timer(const double& beat) const
    {
        if (!parsed) throw LevelNotParsedException();
        double bpm = settings.bpm, b = 0, timer = settings.offset;
        for (size_t i = 0; i < m_setSpeeds.size(); i++)
        {
            timer += bpm2mspb(bpm) * (std::min(beat, m_setSpeeds[i].beat) - b);
            b = m_setSpeeds[i].beat;
            if (m_setSpeeds[i].speedType == Event::GamePlay::SetSpeed::SpeedType::Bpm)
                bpm = m_setSpeeds[i].beatsPerMinute;
            else
                bpm *= m_setSpeeds[i].bpmMultiplier;
            if (beat <= m_setSpeeds[i].beat)
                return timer;
        }
        timer += bpm2mspb(bpm) * (beat - b);
        return timer;
    }

    double Level::timer2beat(double timer) const
    {
        if (!parsed) throw LevelNotParsedException();
        timer -= settings.offset;
        double bpm = settings.bpm, last_beat = 0, beat = 0;
        for (size_t i = 0; i < m_setSpeeds.size(); i++)
        {
            if (timer > bpm2mspb(bpm) * (m_setSpeeds[i].beat - last_beat))
                timer -= bpm2mspb(bpm) * (m_setSpeeds[i].beat - last_beat);
            else
            {
                beat += timer / bpm2mspb(bpm);
                return beat;
            }
            beat += m_setSpeeds[i].beat - last_beat;
            if (m_setSpeeds[i].speedType == Event::GamePlay::SetSpeed::SpeedType::Bpm)
                bpm = m_setSpeeds[i].beatsPerMinute;
            else
                bpm *= m_setSpeeds[i].bpmMultiplier;
            last_beat = m_setSpeeds[i].beat;
        }
        if (timer != 0) beat += timer / bpm2mspb(bpm);
        return beat;
    }

    double Level::getAngle(const size_t& i) const
    {
        if (tiles[i].angle == 999)
            return 0;
        double angle;
        if (tiles[i - 1].angle == 999)
            angle = tiles[i - 2].angle - tiles[i].angle;
        else
            angle = tiles[i - 1].angle - 180 - tiles[i].angle;
        if (!tiles[i - 1].orbit)
            angle *= -1;
        while (angle <= 0) angle += 360;
        while (angle > 360) angle -= 360;
        return angle;
    }

    Point Level::f(double beat) const
    {
        double a, mspb; Point p;
        for (size_t j = 1; j < tiles.size(); j++)
        {
            if (beat < tiles[j].beat) break;
            if (j != tiles.size() - 1 && tiles[j + 1].angle == 999) continue;
            mspb = bpm2mspb(getBpmByBeat(tiles[j].beat));
            if (j == tiles.size() - 1)
                a = beat2timer(beat) - beat2timer(tiles[j].beat);
            else
                a = beat2timer(std::min(beat, tiles[j + 1].beat)) - beat2timer(tiles[j].beat);
            a = std::min(a, mspb * 2) / (mspb * 2);
            p.first += (tiles[j].oPos.first - p.first) * a,
                p.second += (tiles[j].oPos.second - p.second) * a;
        }
        return p;
    }
    
    Level::CameraInfo Level::getCameraInfo(const double& beat) const  // NEED TO OPTIMIZE
    {
        if (!parsed) throw LevelNotParsedException();

        Point position = settings.position, posOff; double rotation = settings.rotation, zoom = settings.zoom;
        double x, y;
        bool turning = false;
        std::string relativeTo = settings.relativeTo;

        for (size_t i = 0; i < m_moveCameras.size(); i++)
        {
            auto& moveCamera = m_moveCameras[i];
            if (beat < moveCamera.beat) break;
            if (moveCamera.duration == 0)
                x = y = 1;
            else
                x = (beat - moveCamera.beat) / moveCamera.duration, y = ease(moveCamera.ease, x);

            if (moveCamera.position.first)
                posOff.first += (*moveCamera.position.first - posOff.first) * y;
            if (moveCamera.position.second)
                posOff.second += (*moveCamera.position.second - posOff.second) * y;
            if (moveCamera.rotation)
                rotation += (*moveCamera.rotation - rotation) * y;
            if (moveCamera.zoom)
                zoom += (*moveCamera.zoom - zoom) * y;

            if (moveCamera.relativeTo)
            {
                if (*moveCamera.relativeTo == "Tile")
                {
                    auto& tile = tiles[moveCamera.floor];
                    if (relativeTo == "Player")
                        position = m_moveCameraPlayerPosVec2[i];
                    position.first += (tile.oPos.first - position.first) * y;
                    position.second += (tile.oPos.second - position.second) * y;
                }
                else if (*moveCamera.relativeTo == "Player" && relativeTo == "Tile")
                {
                    position.first += (m_moveCameraPlayerPosVec[i].first - position.first) * y;
                    position.second += (m_moveCameraPlayerPosVec[i].second - position.second) * y;
                    turning = (y != 1);
                }
                relativeTo = *moveCamera.relativeTo;
            }
        }
        if (!turning && relativeTo == "Player")
        {
            Point p = f(beat);
            position.first = p.first, position.second = p.second;
        }
        // TODO: moveCamera relativeTo Tile
        position.first += posOff.first, position.second += posOff.second;
        //std::cout << position.first << ' ' << position.second << std::endl;
        //for (size_t i = 0; i < m_moveCameras.size(); i++)
        //{
        //    auto& mc = m_moveCameras[i];
        //    if (beat < mc.beat) break;
        //    double x, y;
        //    if (mc.duration == 0)
        //        x = y = 1;
        //    else
        //        x = (beat - mc.beat) / mc.duration, y = ease(mc.ease, x);
        //    if (mc.position.first) posOff.first = *mc.position.first;
        //    if (mc.position.second) posOff.second = *mc.position.second;
        //    if (mc.relativeTo) relativeTo = *mc.relativeTo;
        //    if (mc.rotation) rotation += (*mc.rotation - rotation) * y;
        //    if (mc.zoom) zoom += (*mc.zoom - zoom) * y;
        //    if (relativeTo == "Tile")
        //        position.first = (tiles[mc.floor].pos.first - position.first) * y,
        //        position.second = (tiles[mc.floor].pos.second - position.second) * y;
        //    else if (relativeTo == "Player")
        //    {
        //        std::cout << 1;

        //        if (i == m_moveCameras.size() - 1)
        //        {
        //            double a;
        //            for (size_t j = getTileIndexByBeat(beat) + 1; j < tiles.size() - 1; j++)
        //            {
        //                if (beat < tiles[j].beat) break;
        //                a = std::min((beat - tiles[j].beat) / (tiles[j + 1].beat - tiles[j].beat), 1.0) * 0.5;
        //                position.first += (tiles[j].oPos.first - position.first) * a, position.second += (tiles[j].oPos.second - position.second) * a;
        //            }
        //        }
        //        else
        //        {
        //            double a;
        //            size_t begin = getTileIndexByBeat(beat) + 1,
        //                end = getTileIndexByBeat(m_moveCameras[i + 1].beat);
        //            for (size_t j = begin; j < end; j++)
        //            {
        //                if (beat < tiles[j].beat) break;
        //                a = std::min((beat - tiles[j].beat) / (tiles[j + 1].beat - tiles[j].beat), 1.0) * 0.5;
        //                position.first += (tiles[j].oPos.first - position.first) * a, position.second += (tiles[j].oPos.second - position.second) * a;
        //            }
        //        }
        //    }
        //}
        //position.first += posOff.first, position.second += posOff.second;
         
        // TODO
        //Point p;
        //double timer = beat2timer(beat), diff, this_timer, next_timer = beat2timer(tiles[0].beat), a;
        //for (size_t i = 0; i < tiles.size(); i++)
        //{
        //    this_timer = next_timer;
        //    if (i == tiles.size() - 1)
        //        next_timer = this_timer + 1000;
        //    else
        //        next_timer = beat2timer(tiles[i + 1].beat);
        //    if (timer > next_timer)
        //        diff = next_timer - this_timer;
        //    else
        //        diff = timer - this_timer;
        //    a = std::min(diff / std::min(1000.0, bpm2mspb(get_bpm(tiles[i].beat) / 2)), 1.0);
        //    p.first += (tiles[i].oPos.first - p.first) * a,
        //        p.second += (tiles[i].oPos.second - p.second) * a;

        //    if (timer <= next_timer) break;
        //}
        return { position, rotation, zoom };
    }

    double Level::getTiming(const size_t& index, const double& timer) const
    {
        return timer - beat2timer(tiles[index].beat);
    }

    HitMargin Level::getHitMargin(const size_t& index, const double& timer, const Difficulty& difficulty) const
    {
        if (!parsed) throw LevelNotParsedException();
        const double max_bpm = difficulty == Difficulty::Lenient
            ? 210
            : difficulty == Difficulty::Normal
            ? 330
            : 500,
            ms = bpm2mspb(std::min(max_bpm, getBpmNotIncludingBeat(tiles[index].beat))),
            p = ms / 6,
            lep = ms / 4,
            vle = ms / 3,
            timing = getTiming(index, timer);
        if (timing > vle) return HitMargin::TooLate;
        if (timing > lep) return HitMargin::VeryLate;
        if (timing > p) return HitMargin::LatePerfect;
        if (timing > -p) return HitMargin::Perfect;
        if (timing > -lep) return HitMargin::EarlyPerfect;
        if (timing > -vle) return HitMargin::VeryEarly;
        return HitMargin::TooEarly;
    }
    bool Level::isParsed() const
    {
        return parsed;
    }
}