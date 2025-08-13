#include "Level.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <optional>
#include <ranges>
#include <rapidjson/prettywriter.h>

#include "Utils.h"
#include "rapidjson/document.h"

constexpr double positiveRemainder(const double a, const double b)
{
    assert(b > 0.0 && "Cannot calculate remainder with non-positive divisor");
    const double val = a - static_cast<double>(static_cast<int>(a / b)) * b;
    return val >= 0.0 ? val : val + b;
}

namespace AdoCpp
{
    Settings::Settings(const rapidjson::Value& jsonSettings) { *this = fromJson(jsonSettings); }
    Settings Settings::fromJson(const rapidjson::Value& jsonSettings)
    {
        Settings settings;

        settings.artist = jsonSettings["artist"].GetString();
        settings.song = jsonSettings["song"].GetString();
        settings.author = jsonSettings["author"].GetString();
        settings.separateCountdownTime = toBool(jsonSettings["separateCountdownTime"]);
        settings.songFilename = jsonSettings["songFilename"].GetString();
        settings.bpm = jsonSettings["bpm"].GetDouble();
        settings.offset = jsonSettings["offset"].GetDouble();
        settings.pitch = jsonSettings["pitch"].GetDouble();

        settings.hitsound = cstr2hitsound(jsonSettings["hitsound"].GetString());
        settings.hitsoundVolume =
            jsonSettings[jsonSettings.HasMember("hitsoundVolume") ? "hitsoundVolume" : "hitsoundSingle"].GetDouble();

        settings.countdownTicks =
            jsonSettings.HasMember("countdownTicks") ? jsonSettings["countdownTicks"].GetDouble() : 4;

        settings.trackColorType = cstr2trackColorType(jsonSettings["trackColorType"].GetString());
        settings.trackColor = Color(jsonSettings["trackColor"].GetString());
        settings.secondaryTrackColor = Color(jsonSettings["secondaryTrackColor"].GetString());
        settings.trackColorAnimDuration = jsonSettings["trackColorAnimDuration"].GetDouble();
        settings.trackColorPulse = cstr2trackColorPulse(jsonSettings["trackColorPulse"].GetString());
        settings.trackPulseLength = jsonSettings["trackPulseLength"].GetUint();
        settings.trackStyle = cstr2trackStyle(jsonSettings["trackStyle"].GetString());

        settings.trackAnimation = cstr2trackAnimation(jsonSettings["trackAnimation"].GetString());
        settings.beatsAhead = jsonSettings["beatsAhead"].GetDouble();
        settings.trackDisappearAnimation =
            cstr2trackDisappearAnimation(jsonSettings["trackDisappearAnimation"].GetString());
        settings.beatsBehind = jsonSettings["beatsBehind"].GetDouble();

        settings.backgroundColor = Color(jsonSettings["backgroundColor"].GetString());

        if (jsonSettings.HasMember("unscaledSize"))
            settings.unscaledSize = jsonSettings["unscaledSize"].GetDouble();

        settings.relativeTo = cstr2relativeToCamera(jsonSettings["relativeTo"].GetString());
        settings.position = Vector2lf(jsonSettings["position"][0].GetDouble(), jsonSettings["position"][1].GetDouble());
        settings.rotation = jsonSettings["rotation"].GetDouble();
        settings.zoom = jsonSettings["zoom"].GetDouble();

        settings.stickToFloors = toBool(jsonSettings["stickToFloors"]);

        return settings;
    }
    std::unique_ptr<rapidjson::GenericValue<rapidjson::UTF8<>>>
    Settings::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        auto val = std::make_unique<rapidjson::Value>(rapidjson::kObjectType);
        val->AddMember("version", 15, alloc);
        rapidjson::Value vArtist;
        vArtist.SetString(artist.c_str(), artist.length(), alloc);
        val->AddMember("artist", vArtist, alloc);
        rapidjson::Value vSong;
        vSong.SetString(artist.c_str(), artist.length(), alloc);
        val->AddMember("song", vSong, alloc);
        rapidjson::Value vAuthor;
        vAuthor.SetString(author.c_str(), author.length(), alloc);
        val->AddMember("author", vAuthor, alloc);
        val->AddMember("separateCountdownTime", separateCountdownTime, alloc);
        val->AddMember("countdownTicks", countdownTicks, alloc);
        rapidjson::Value vSongFilename;
        vSongFilename.SetString(songFilename.c_str(), songFilename.length(), alloc);
        val->AddMember("songFilename", vSongFilename, alloc);
        val->AddMember("bpm", bpm, alloc);
        val->AddMember("volume", volume, alloc);
        val->AddMember("offset", offset, alloc);
        val->AddMember("pitch", pitch, alloc);
        val->AddMember("hitsound", rapidjson::StringRef(hitsound2cstr(hitsound)), alloc);
        val->AddMember("hitsoundVolume", hitsoundVolume, alloc);
        val->AddMember("trackColorType", rapidjson::StringRef(trackColorType2cstr(trackColorType)), alloc);
        rapidjson::Value vTrackColor;
        std::string sTrackColor = trackColor.toString(false, false, Color::ToStringAlphaMode::Auto);
        vTrackColor.SetString(sTrackColor.c_str(), sTrackColor.length(), alloc);
        val->AddMember("trackColor", vTrackColor, alloc);
        rapidjson::Value vSSTC;
        std::string sSSTC = trackColor.toString(false, false, Color::ToStringAlphaMode::Auto);
        vSSTC.SetString(sSSTC.c_str(), sSSTC.length(), alloc);
        val->AddMember("secondaryTrackColor", vSSTC, alloc);
        val->AddMember("trackColorAnimDuration", trackColorAnimDuration, alloc);
        val->AddMember("trackColorPulse", rapidjson::StringRef(trackColorPulse2cstr(trackColorPulse)), alloc);
        val->AddMember("trackPulseLength", trackPulseLength, alloc);
        val->AddMember("trackStyle", rapidjson::StringRef(trackStyle2cstr(trackStyle)), alloc);
        val->AddMember("trackAnimation", rapidjson::StringRef(trackAnimation2cstr(trackAnimation)), alloc);
        val->AddMember("beatsAhead", beatsAhead, alloc);
        val->AddMember("trackDisappearAnimation",
                      rapidjson::StringRef(trackDisappearAnimation2cstr(trackDisappearAnimation)), alloc);
        val->AddMember("beatsBehind", beatsBehind, alloc);
        rapidjson::Value vBGC;
        std::string sBGC = backgroundColor.toString(false, false, Color::ToStringAlphaMode::Auto);
        vBGC.SetString(sBGC.c_str(), sBGC.length(), alloc);
        val->AddMember("backgroundColor", vBGC, alloc);
        val->AddMember("stickToFloors", stickToFloors, alloc);
        // val->AddMember("unscaledSize", unscaledSize, alloc);
        val->AddMember("relativeTo", rapidjson::StringRef(relativeToCamera2cstr(relativeTo)), alloc);
        rapidjson::Value vPos(rapidjson::kArrayType);
        vPos.PushBack(position.x, alloc), vPos.PushBack(position.y, alloc);
        val->AddMember("position", vPos, alloc);
        val->AddMember("rotation", rotation, alloc);
        val->AddMember("zoom", zoom, alloc);
        return val;
    }
    std::unique_ptr<rapidjson::Document> Settings::intoJson() const
    {
        auto doc = std::make_unique<rapidjson::Document>();
        doc->CopyFrom(*intoJson(doc->GetAllocator()), doc->GetAllocator());
        return doc;
    }
    void Settings::apply(Tile& tile) const
    {
        tile.editorPos = tile.pos.o = {0, 0}, tile.stickToFloors = stickToFloors, tile.trackAnimationFloor = 0,

        // clang-format off
        tile.trackAnimation           = trackAnimation,          tile.beatsAhead  = beatsAhead,
        tile.trackDisappearAnimation  = trackDisappearAnimation, tile.beatsBehind = beatsBehind;

        tile.trackColorType.o         = trackColorType;
        tile.trackColor.o             = trackColor;
        tile.secondaryTrackColor.o    = secondaryTrackColor;
        tile.trackColorAnimDuration.o = trackColorAnimDuration;
        tile.trackStyle.o             = trackStyle;
        tile.trackColorPulse.o        = trackColorPulse;
        tile.trackPulseLength.o       = trackPulseLength;

        tile.midspinHitsound          = tile.hitsound       = hitsound,
        tile.midspinHitsoundVolume    = tile.hitsoundVolume = hitsoundVolume;
        // clang-format on
    }

    Level::Level(const rapidjson::Document& document) { fromJson(document); }

    Level::Level(std::ifstream& ifs) { fromFile(ifs); }

    Level::Level(const std::filesystem::path& path) { fromFile(path); }

    static double deg2rad(const double deg) { return deg * 3.141592653589793 / 180; }

    void Level::clear()
    {
        parsed = false;
        settings = Settings();
        tiles.clear();
        m_processedDynamicEvents.clear();
        m_moveCameraDatas.clear();
        m_setSpeeds.clear();
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
        clear();
        assert(document.IsObject());

        tiles.emplace_back(0);
        if (document.HasMember("angleData"))
        {
            for (const auto& angle : document["angleData"].GetArray())
                tiles.emplace_back(angle.GetDouble());
        }
        else
        {
            assert(document.HasMember("pathData") && "The json must have either 'angleData' or 'pathData'");
            for (const auto& path : std::string(document["pathData"].GetString()))
                tiles.emplace_back(path2angle(path));
        }

        settings = Settings::fromJson(document["settings"]);

        for (const auto& eventData : document["actions"].GetArray())
        {
            try
            {
                if (auto event = std::shared_ptr<Event::Event>(Event::newEvent(eventData)))
                    tiles[event->floor].events.push_back(event);
            }
            catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }

    void Level::fromFile(std::ifstream& ifs)
    {
        rapidjson::Document document;
        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::AutoUTFInputStream<unsigned, rapidjson::IStreamWrapper> eis(isw);
        document.ParseStream<rapidjson::kParseValidateEncodingFlag | rapidjson::kParseCommentsFlag |
                                 rapidjson::kParseTrailingCommasFlag | rapidjson::kParseNanAndInfFlag
                             //| rapidjson::kParseFullPrecisionFlag
                             ,
                             rapidjson::AutoUTF<unsigned>>(eis);
        if (document.HasParseError())
            throw LevelJsonException(document.GetParseError());
        fromJson(document);
    }

    void Level::fromFile(const std::filesystem::path& path)
    {
        std::ifstream ifs(path);
        if (!ifs.is_open())
            throw LevelCouldNotOpenFileException();
        fromFile(ifs);
        ifs.close();
    }
    std::unique_ptr<rapidjson::Value> Level::intoJson(rapidjson::Document::AllocatorType& alloc) const
    {
        auto val = std::make_unique<rapidjson::Value>(rapidjson::kObjectType);
        {
            rapidjson::Value angleData(rapidjson::kArrayType);
            for (size_t i = 1; i < tiles.size(); i++)
            {
                if (const auto& tile = tiles[i]; static_cast<int>(tile.angle.deg()) == tile.angle.deg())
                    angleData.PushBack(static_cast<int>(tile.angle.deg()), alloc);
                else
                    angleData.PushBack(tile.angle.deg(), alloc);
            }
            val->AddMember("angleData", angleData, alloc);
        }
        {
            val->AddMember("settings", *settings.intoJson(alloc), alloc);
        }
        {
            rapidjson::Value actions(rapidjson::kArrayType);
            for (const auto& tile : tiles)
                for (const auto& event : tile.events)
                    actions.PushBack(*event->intoJson(alloc), alloc);

            val->AddMember("actions", actions, alloc);
        }
        {
            rapidjson::Value decorations(rapidjson::kArrayType);
            val->AddMember("decorations", decorations, alloc);
        }

        return val;
    }
    std::unique_ptr<rapidjson::Document> Level::intoJson() const
    {
        auto doc = std::make_unique<rapidjson::Document>();
        doc->CopyFrom(*intoJson(doc->GetAllocator()), doc->GetAllocator());
        return doc;
    }

    void Level::parse(const size_t floorStart, const bool basic, const bool force)
    {
        if (parsed && !force && !onlyBasic)
            return;
        assert(tiles.size() >= 2 && "AdoCpp::Level class must have at least two tiles to parse");
        parsed = true, onlyBasic = basic;
        parseTiles(floorStart);
        parseSetSpeed();
        if (basic)
        {
            tiles[0].beat = tiles[0].seconds = -std::numeric_limits<double>::infinity();
            parsed = true;
            return;
        }
        std::vector<Event::DynamicEvent*> dynamicEvents;
        std::vector<std::vector<Event::Modifiers::RepeatEvents*>> vecRe{tiles.size()};
        parseDynamicEvents(dynamicEvents, vecRe);
        if (!m_disableAnimateTrack)
            parseAnimateTrack();
        parseRepeatEvents(dynamicEvents, vecRe);
        m_processedDynamicEvents.sort([](const auto& a, const auto& b) { return a->beat < b->beat; }); // stable sort
        parseMoveTrackData();

        tiles[0].beat = tiles[0].seconds = -std::numeric_limits<double>::infinity();
        parsed = true;
    }
    void Level::update()
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        for (size_t i = 0; i < tiles.size(); i++)
        {
            auto& tile = tiles[i];
            tile.pos.o2c(), tile.scale.o2c(), tile.rotation.o2c(), tile.opacity = 100;
            tile.trackColorType.o2c(), tile.trackColor.o2c(), tile.secondaryTrackColor.o2c(),
                tile.trackColorAnimDuration.o2c(), tile.trackStyle.o2c(), tile.trackColorPulse.o2c(),
                tile.trackPulseLength.o2c();
            updateTileColor(0, i);
        }
    }
    void Level::update(const double seconds)
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        update();
        for (const auto& dynamicEvent : m_processedDynamicEvents)
        {
            if (seconds < dynamicEvent->seconds)
                break;
            if (auto recolorTrack = std::dynamic_pointer_cast<Event::Track::RecolorTrack>(dynamicEvent))
                updateTileColorInfo(recolorTrack.get());
        }

        for (size_t i = 0; i < tiles.size(); i++)
        {
            updateTileColor(seconds, i);
            updateTilePos(seconds, i);
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
    void Level::insertTile(const size_t floor, const Tile& tile)
    {
        parsed = false;
        tiles.insert(tiles.begin() + floor, tile); // NOLINT(*-narrowing-conversions)
    }
    void Level::insertTile(const size_t floor, const double angle)
    {
        parsed = false;
        tiles.emplace(tiles.begin() + floor, angle); // NOLINT(*-narrowing-conversions)
    }
    void Level::changeTileAngle(const size_t floor, const double angle)
    {
        parsed = false;
        tiles[floor].angle = degrees(angle);
    }
    void Level::eraseTile(const size_t first, const size_t last)
    {
        parsed = false;
        tiles.erase(tiles.begin() + first, // NOLINT(*-narrowing-conversions)
                    tiles.begin() + std::min(last, tiles.size())); // NOLINT(*-narrowing-conversions)
    }
    void Level::pushBackTile(const Tile& tile)
    {
        parsed = false;
        tiles.push_back(tile);
    }
    void Level::pushBackTile(double angle)
    {
        parsed = false;
        tiles.emplace_back(angle);
    }
    void Level::popBackTile()
    {
        parsed = false;
        tiles.pop_back();
    }

    size_t Level::rel2absIndex(const size_t baseIndex, const RelativeIndex relativeIndex) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
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
        }
        unreachable();
    }
    double Level::getPlanetsDir(const size_t floor, const double seconds) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        const double bpm = getBpm([&floor, &seconds](const Event::GamePlay::SetSpeed& ss)
                                  { return ss.floor <= floor && ss.seconds <= seconds; }),
                     spb = bpm2crotchet(bpm);
        double angle;
        if (floor == 0)
        {
            angle = -seconds / spb * 180;
        }
        else
        {
            if (tiles[floor].angle.deg() == 999)
                angle = tiles[floor - 1].angle.deg();
            else
                angle = tiles[floor].angle.deg() - 180;
            if (tiles[floor].orbit == Clockwise)
                angle -= ((seconds - tiles[floor].seconds) / spb) * 180;
            else
                angle += ((seconds - tiles[floor].seconds) / spb) * 180;
        }
        return angle;
    }
    std::pair<Vector2lf, Vector2lf> Level::getPlanetsPos(const size_t floor, const double seconds) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        Vector2lf p2, p1 = p2 = tiles[floor].stickToFloors ? tiles[floor].pos.c : tiles[floor].pos.o;
        const double angle = getPlanetsDir(floor, seconds);
        p2.x += cos(deg2rad(angle)), p2.y += sin(deg2rad(angle));
        if (isFirePlanetStatic(floor))
            return std::make_pair(p1, p2);
        return std::make_pair(p2, p1);
    }
    bool Level::isFirePlanetStatic(const size_t floor) { return floor % 2 == 0; }
    size_t Level::getFloorByBeat(const double beat) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        return std::upper_bound(tiles.begin() + 1, tiles.end(), beat,
                                [](const double& val, const Tile& e) -> bool { return val < e.beat; }) -
            (tiles.begin() + 1);
    }
    size_t Level::getFloorBySeconds(const double seconds) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        return std::upper_bound(tiles.begin() + 1, tiles.end(), seconds,
                                [](const double& val, const Tile& e) -> bool { return val < e.seconds; }) -
            (tiles.begin() + 1);
    }
    double Level::getBpm(const std::function<bool(const Event::GamePlay::SetSpeed&)>& func) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
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
    double Level::getBpmByBeat(const double beat) const
    {
        return getBpm([&](const Event::GamePlay::SetSpeed& ss) { return beat >= ss.beat; });
    }
    double Level::getBpmBySeconds(const double seconds) const
    {
        return getBpm([&](const Event::GamePlay::SetSpeed& ss) { return seconds >= ss.seconds; });
    }
    double Level::getBpmExcludingBeat(const double beat) const
    {
        return getBpm([&](const Event::GamePlay::SetSpeed& ss) { return beat > ss.beat; });
    }
    double Level::getBpmForDynamicEvent(const size_t floor, const double angleOffset) const
    {
        return getBpm([&](const Event::GamePlay::SetSpeed& ss)
                      { return floor > ss.floor || (floor == ss.floor && angleOffset >= ss.angleOffset); });
    }
    double Level::beat2seconds(const double beat) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
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
        assert(parsed && "AdoCpp::Level class is not parsed");
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

    double Level::getAngle(const size_t floor) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        if (tiles[floor].angle.deg() == 999)
            return 0;
        double angle;
        if (tiles[floor - 1].angle.deg() == 999)
            angle = tiles[floor - 2].angle.deg() - tiles[floor].angle.deg();
        else
            angle = tiles[floor - 1].angle.deg() - 180 - tiles[floor].angle.deg();
        if (tiles[floor - 1].orbit == CounterClockwise)
            angle *= -1;
        angle = positiveRemainder(angle, 360);
        if (angle == 0)
            angle = 360;
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
        assert(parsed && "AdoCpp::Level class is not parsed");
        m_camera = Camera();
        m_moveCameraDatas.clear();
        double rotEndSec, zoomEndSec, xEndSec, yEndSec,
            relEndSec = xEndSec = yEndSec = rotEndSec = zoomEndSec = std::numeric_limits<double>::infinity();
        m_moveCameraDatas.emplace_back(0, 0, -settings.countdownTicks,
                                       -settings.countdownTicks * bpm2crotchet(settings.bpm), 0.0, settings.relativeTo,
                                       false, relEndSec, Vector2lf(), OptionalPoint(), xEndSec, yEndSec,
                                       settings.rotation, rotEndSec, settings.zoom, zoomEndSec, Easing::Linear);
        for (const auto& m_processedDynamicEvent : std::ranges::reverse_view(m_processedDynamicEvents))
        {
            const auto mc = std::dynamic_pointer_cast<Event::Visual::MoveCamera>(m_processedDynamicEvent);
            if (mc == nullptr)
                continue;
            m_moveCameraDatas.emplace(m_moveCameraDatas.begin() + 1, mc->floor, mc->angleOffset, mc->beat, mc->seconds,
                                      mc->duration, mc->relativeTo, false, 114514, Vector2lf(), mc->position, xEndSec,
                                      yEndSec, mc->rotation, rotEndSec, mc->zoom, zoomEndSec, mc->ease);
            if (mc->relativeTo)
            {
                // relEndSec = mc->seconds; // i hate this line
                if (*mc->relativeTo == RelativeToCamera::LastPosition)
                    xEndSec = yEndSec = mc->seconds;
            }
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
            if (!it->duplicatedRelPlayer && it->relativeTo.has_value())
                lastRel = *it->relativeTo;
        }
        for (auto& m_moveCameraData : std::ranges::reverse_view(m_moveCameraDatas))
        {
            m_moveCameraData.relEndSec = relEndSec;
            if (!m_moveCameraData.duplicatedRelPlayer)
                relEndSec = m_moveCameraData.seconds;
        }
    }

    void Level::updateCamera(const double seconds, const size_t floor) // FIXME
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        Vector2lf pos, posOff;
        double rot{}, zoom{};
        for (auto& data : m_moveCameraDatas)
        {
            const double bpm = getBpmForDynamicEvent(data.floor, data.angleOffset), spb = bpm2crotchet(bpm);
            if (seconds < data.seconds)
                break;

            auto calcX = [&seconds, &data, &spb](const double endSec)
            { return data.duration != 0.0 ? (std::min(seconds, endSec) - data.seconds) / spb / data.duration : 1.0; };

            if (data.relativeTo && !data.duplicatedRelPlayer)
            {
                const double x = calcX(data.relEndSec), y = ease(data.ease, x);
                using enum RelativeToCamera;
                switch (*data.relativeTo)
                {
                case Player:
                    {
                        if (seconds > data.relEndSec)
                        {
                            pos = data.playerLastPos;
                            break;
                        }

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
                            const Vector2lf v = targetPos - m_camera.player;
                            if (const Vector2lf n = v.normalized() * delta * speed;
                                (m_camera.player - targetPos).lengthSquared() > n.lengthSquared())
                                m_camera.player += n;
                            else
                                m_camera.player = targetPos;
                        }

                        pos += (m_camera.player - pos) * y;
                        data.playerLastPos = pos;
                        break;
                    }
                case Tile:
                    pos += (tiles[data.floor].pos.o - pos) * y;
                    if (seconds <= data.relEndSec)
                        m_camera.player = pos;
                    break;
                case Global:
                    // pos += (Vector2lf(0, 0) - pos) * y;
                    // pos += -pos * y;
                    pos *= (1 - y);
                    if (seconds <= data.relEndSec)
                        m_camera.player = pos;
                    break;
                case LastPosition:
                    pos += posOff;
                    posOff = Vector2lf(0, 0);
                    if (seconds <= data.relEndSec)
                        m_camera.player = pos;
                    break;
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
    bool Level::disableAnimateTrack() const { return m_disableAnimateTrack; }
    void Level::disableAnimateTrack(const bool disable)
    {
        if (m_disableAnimateTrack != disable)
            parsed = false;
        m_disableAnimateTrack = disable;
    }

    double Level::getTiming(const size_t floor, const double seconds) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        return seconds - tiles[floor].seconds;
    }

    Level::TimingBoundary Level::getTimingBoundary(const size_t floor, const Difficulty difficulty) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        double bpm = settings.bpm, k = 1;
        for (const auto& setSpeed : m_setSpeeds)
        {
            if (tiles[floor].beat <= setSpeed->beat)
                break;
            if (setSpeed->speedType == Event::GamePlay::SetSpeed::SpeedType::Bpm)
                bpm = setSpeed->beatsPerMinute, k = 1;
            else
                bpm *= setSpeed->bpmMultiplier, k *= setSpeed->bpmMultiplier;
        }
        k = 1;
        using enum Difficulty;
        const double seconds = std::max(bpm2crotchet(bpm),
                                        difficulty == Lenient      ? 91.0 * 3 / 1000
                                            : difficulty == Normal ? 65.0 * 3 / 1000
                                                                   : 40.0 * 3 / 1000),
                     p = std::max(25.0 / 1000, seconds / 6 / k), lep = std::max(25.0 / 1000, seconds / 4 / k),
                     vle = std::max(25.0 / 1000, seconds / 3 / k);
        return {p, lep, vle};
    }

    HitMargin Level::getHitMargin(const size_t floor, const double seconds, const Difficulty difficulty) const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        const auto [p, lep, vle] = getTimingBoundary(floor, difficulty);
        const double timing = getTiming(floor, seconds);
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
    bool Level::isParsed() const noexcept { return parsed; }

    Level::CameraValue Level::cameraValue() const
    {
        assert(parsed && "AdoCpp::Level class is not parsed");
        return {m_camera.position, m_camera.rotation, m_camera.zoom};
    }

    void Level::parseTiles(const size_t beginFloor)
    {
        // clang-format off
        std::vector<bool>                                          twirls(tiles.size());
        std::vector<double>                                        pauses(tiles.size());
        std::vector<std::shared_ptr<Event::GamePlay::SetHitsound>> setHitsounds(tiles.size());
        std::vector<std::shared_ptr<Event::Track::PositionTrack>>  positionTracks(tiles.size());
        std::vector<std::shared_ptr<Event::Track::ColorTrack>>     colorTracks(tiles.size());
        std::vector<std::shared_ptr<Event::Track::AnimateTrack>>   animateTracks(tiles.size());
        std::vector<std::shared_ptr<Event::Dlc::Hold>>             holds(tiles.size());
        for (size_t floor = beginFloor; floor < tiles.size(); floor++)
        {
            for (const auto& event : tiles[floor].events)
            {
                event->floor = floor;
                if (!event->active)
                    continue;

                if (typeid(*event.get()) == typeid(Event::GamePlay::Twirl))
                    twirls[event->floor] = true;
                else if (const auto pause                = std::dynamic_pointer_cast<Event::GamePlay::Pause>(event))
                    pauses[pause->floor]                 = pause->duration;
                else if (const auto setHitsound          = std::dynamic_pointer_cast<Event::GamePlay::SetHitsound>(event))
                    setHitsounds[setHitsound->floor]     = setHitsound;

                else if (const auto positionTrack        = std::dynamic_pointer_cast<Event::Track::PositionTrack>(event))
                    positionTracks[positionTrack->floor] = positionTrack;
                else if (const auto colorTrack           = std::dynamic_pointer_cast<Event::Track::ColorTrack>(event))
                    colorTracks[colorTrack->floor]       = colorTrack;
                else if (const auto animateTrack         = std::dynamic_pointer_cast<Event::Track::AnimateTrack>(event))
                    animateTracks[animateTrack->floor]   = animateTrack;
                else if (const auto hold                 = std::dynamic_pointer_cast<Event::Dlc::Hold>(event))
                    holds[hold->floor]                   = hold;
            }
        }
        // clang-format on
        tiles[0].orbit = Clockwise, tiles[0].beat = 0, settings.apply(tiles[0]);
        Vector2lf nextPosOff;
        for (size_t i = beginFloor; i < tiles.size(); i++)
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
                    if (i == 1)
                        angle -= 180;
                    const double beat = angle / 180 + pauses[i - 1] + (holds[i - 1] ? holds[i - 1]->duration * 2 : 0);
                    tiles[i].beat = tiles[i - 1].beat + beat;
                }
            }

            // Tile's position
            if (i != 0)
            {
                tiles[i].stickToFloors = tiles[i - 1].stickToFloors;
                tiles[i].pos.o = tiles[i - 1].pos.o + nextPosOff;
                tiles[i].editorPos = tiles[i - 1].editorPos + nextPosOff;
                const double angle =
                    tiles[i].angle.deg() == 999 ? (tiles[i - 1].angle + degrees(180)).rad() : tiles[i].angle.rad();
                const double dx = cos(angle), dy = sin(angle);
                tiles[i].pos.o.x += dx, tiles[i].editorPos.x += dx;
                tiles[i].pos.o.y += dy, tiles[i].editorPos.y += dy;
            }
            nextPosOff = {0, 0};
            if (positionTracks[i])
            {
                tiles[i].editorPos += positionTracks[i]->positionOffset;
                if (positionTracks[i]->justThisTile && i != tiles.size() - 1)
                    nextPosOff = -positionTracks[i]->positionOffset;
                if (!positionTracks[i]->editorOnly)
                    tiles[i].pos.o += positionTracks[i]->positionOffset;
                if (positionTracks[i]->stickToFloors)
                    tiles[i].stickToFloors = *positionTracks[i]->stickToFloors;
            }

            // Tile's color
            // clang-format off
            if (i != 0)
            {
                tiles[i].trackColorType.o         = tiles[i - 1].trackColorType.o;
                tiles[i].trackColor.o             = tiles[i - 1].trackColor.o;
                tiles[i].secondaryTrackColor.o    = tiles[i - 1].secondaryTrackColor.o;
                tiles[i].trackColorAnimDuration.o = tiles[i - 1].trackColorAnimDuration.o;
                tiles[i].trackStyle.o             = tiles[i - 1].trackStyle.o;
                tiles[i].trackColorPulse.o        = tiles[i - 1].trackColorPulse.o;
                tiles[i].trackPulseLength.o       = tiles[i - 1].trackPulseLength.o;
            }
            if (colorTracks[i])
            {
                tiles[i].trackColorType.o         = colorTracks[i]->trackColorType;
                tiles[i].trackColor.o             = colorTracks[i]->trackColor;
                tiles[i].secondaryTrackColor.o    = colorTracks[i]->secondaryTrackColor;
                tiles[i].trackColorAnimDuration.o = colorTracks[i]->trackColorAnimDuration;
                tiles[i].trackStyle.o             = colorTracks[i]->trackStyle;
                tiles[i].trackColorPulse.o        = colorTracks[i]->trackColorPulse; //
                tiles[i].trackPulseLength.o       = colorTracks[i]->trackPulseLength; //
            }

            // Tile's animation
            if (i != 0)
            {
                tiles[i].trackAnimationFloor     = tiles[i - 1].trackAnimationFloor;
                tiles[i].trackAnimation          = tiles[i - 1].trackAnimation;
                tiles[i].beatsAhead              = tiles[i - 1].beatsAhead;
                tiles[i].trackDisappearAnimation = tiles[i - 1].trackDisappearAnimation;
                tiles[i].beatsBehind             = tiles[i - 1].beatsBehind;
            } // clang-format on
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

            // Tile's hitsound
            // clang-format off
            if (i != 0)
            {
                tiles[i].hitsound              = tiles[i - 1].hitsound;
                tiles[i].hitsoundVolume        = tiles[i - 1].hitsoundVolume;
                tiles[i].midspinHitsound       = tiles[i - 1].midspinHitsound;
                tiles[i].midspinHitsoundVolume = tiles[i - 1].midspinHitsoundVolume;
            }
            if (setHitsounds[i])
            {
                switch (setHitsounds[i]->gameSound)
                {
                case Event::GamePlay::SetHitsound::GameSound::Hitsound:
                    tiles[i].hitsound       = setHitsounds[i]->hitsound;
                    tiles[i].hitsoundVolume = setHitsounds[i]->hitsoundVolume;
                    break;
                case Event::GamePlay::SetHitsound::GameSound::Midspin:
                    tiles[i].midspinHitsound       = setHitsounds[i]->hitsound;
                    tiles[i].midspinHitsoundVolume = setHitsounds[i]->hitsoundVolume;
                    break;
                }
            }
            // clang-format on
        }
        tiles[0].beat = -settings.countdownTicks;
    }
    void Level::parseSetSpeed()
    {
        m_setSpeeds.clear();
        for (const auto& tile : tiles)
        {
            for (const auto& event : tile.events)
            {
                if (const auto setSpeed = std::dynamic_pointer_cast<Event::GamePlay::SetSpeed>(event))
                    if (event->active)
                        setSpeed->beat = tiles[setSpeed->floor].beat + setSpeed->angleOffset / 180,
                        m_setSpeeds.push_back(setSpeed);
            }
        }
        for (auto& tile : tiles)
            tile.seconds = beat2seconds(tile.beat);
    }
    void Level::parseDynamicEvents(std::vector<Event::DynamicEvent*>& dynamicEvents,
                                   std::vector<std::vector<Event::Modifiers::RepeatEvents*>>& vecRe)
    {
        m_processedDynamicEvents.clear();

        for (const auto& tile : tiles)
        {
            for (const auto& event : tile.events)
            {
                if (!event->active)
                    continue;
                if (auto dynamicEventPtr = std::dynamic_pointer_cast<Event::DynamicEvent>(event))
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
                        const double bpm = getBpmForDynamicEvent(dynamicEventPtr->floor, dynamicEventPtr->angleOffset),
                                     spb = bpm2crotchet(bpm);
                        dynamicEventPtr->seconds =
                            tiles[dynamicEventPtr->floor].seconds + dynamicEventPtr->angleOffset / 180 * spb;
                        dynamicEventPtr->beat = seconds2beat(dynamicEventPtr->seconds);
                    }

                    dynamicEvents.push_back(dynamicEventPtr.get());
                    m_processedDynamicEvents.push_back(dynamicEventPtr);
                }
                if (auto repeatEvents = std::dynamic_pointer_cast<Event::Modifiers::RepeatEvents>(event))
                {
                    vecRe[repeatEvents->floor].push_back(repeatEvents.get());
                }
            }
        }
    }
    void Level::parseAnimateTrack()
    {
        // AnimateTrack // FIXME
        for (size_t i = 0; i < tiles.size(); i++)
        {
            const double spb = bpm2crotchet(getBpmByBeat(tiles[tiles[i].trackAnimationFloor].beat)),
                         secondsAhead = tiles[i].beatsAhead * spb, secondsBehind = tiles[i].beatsBehind * spb;
            if (i != 0)
            {
                // TODO complete the track animation & disappear animation
                switch (tiles[i].trackAnimation)
                {
                case TrackAnimation::None:
                    break;
                case TrackAnimation::Fade:
                default:
                    {
                        const auto mtHide = std::make_shared<Event::Track::MoveTrack>(),
                                   mtAppear = std::make_shared<Event::Track::MoveTrack>();
                        mtHide->floor = mtAppear->floor = i;
                        mtHide->startTile = mtHide->endTile = mtAppear->startTile = mtAppear->endTile =
                            RelativeIndex(0, ThisTile);
                        mtHide->beat = mtHide->seconds = -std::numeric_limits<double>::infinity();
                        mtHide->opacity = 0;
                        mtAppear->seconds = tiles[i].seconds - secondsAhead;
                        mtAppear->beat = seconds2beat(mtAppear->seconds);
                        mtAppear->duration = 0.5;
                        mtAppear->opacity = 100;
                        mtHide->generated = mtAppear->generated = true;
                        m_processedDynamicEvents.push_front(mtHide);
                        m_processedDynamicEvents.push_front(mtAppear);
                        break;
                    }
                case TrackAnimation::Grow_Spin:
                    {
                        const auto mtHide = std::make_shared<Event::Track::MoveTrack>(),
                                   mtAppear = std::make_shared<Event::Track::MoveTrack>();
                        mtHide->floor = mtAppear->floor = i;
                        mtHide->startTile = mtHide->endTile = mtAppear->startTile = mtAppear->endTile =
                            RelativeIndex(0, ThisTile);
                        mtHide->beat = mtHide->seconds = -std::numeric_limits<double>::infinity();
                        mtHide->rotationOffset = -180;
                        mtHide->scale = OptionalPoint(std::make_optional(0.0), std::make_optional(0.0));
                        mtAppear->seconds = tiles[i].seconds - secondsAhead;
                        mtAppear->beat = seconds2beat(mtAppear->seconds);
                        mtAppear->duration = 0.5;
                        mtAppear->rotationOffset = 0;
                        mtAppear->scale = OptionalPoint(std::make_optional(100.0), std::make_optional(100.0));
                        mtHide->generated = mtAppear->generated = true;
                        m_processedDynamicEvents.push_front(mtHide);
                        m_processedDynamicEvents.push_front(mtAppear);
                        break;
                    }
                }
            }
            if (i != tiles.size() - 1)
            {
                switch (tiles[i].trackDisappearAnimation)
                {
                case TrackDisappearAnimation::None:
                    break;
                case TrackDisappearAnimation::Fade:
                default:
                    {
                        const auto mtDisappear = std::make_shared<Event::Track::MoveTrack>();
                        mtDisappear->floor = i;
                        mtDisappear->startTile = mtDisappear->endTile = RelativeIndex(0, ThisTile);
                        mtDisappear->seconds = tiles[i + 1].seconds + secondsBehind;
                        mtDisappear->beat = seconds2beat(mtDisappear->seconds);
                        mtDisappear->duration = 0.5;
                        mtDisappear->opacity = 0;
                        mtDisappear->generated = true;
                        m_processedDynamicEvents.insert(m_processedDynamicEvents.begin(), mtDisappear);
                        break;
                    }
                case TrackDisappearAnimation::Shrink_Spin:
                    {
                        const auto mtDisappear = std::make_shared<Event::Track::MoveTrack>();
                        mtDisappear->floor = i;
                        mtDisappear->startTile = mtDisappear->endTile = RelativeIndex(0, ThisTile);
                        mtDisappear->seconds = tiles[i + 1].seconds + secondsBehind;
                        mtDisappear->beat = seconds2beat(mtDisappear->seconds);
                        mtDisappear->duration = 0.5;
                        mtDisappear->rotationOffset = 180;
                        mtDisappear->scale = OptionalPoint(std::make_optional(0.0), std::make_optional(0.0));
                        mtDisappear->generated = true;
                        m_processedDynamicEvents.insert(m_processedDynamicEvents.begin(), mtDisappear);
                        break;
                    }
                }
            }
        }
    }
    void Level::parseRepeatEvents(const std::vector<Event::DynamicEvent*>& dynamicEvents,
                                  const std::vector<std::vector<Event::Modifiers::RepeatEvents*>>& vecRe)
    {
        for (const auto& event : dynamicEvents)
            for (const auto& repeatEvents : vecRe[event->floor])
                for (const auto& tag : repeatEvents->tag)
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
                                const auto eventClone = event->clone();
                                eventClone->seconds += gap * static_cast<double>(i);
                                eventClone->beat = seconds2beat(eventClone->seconds);
                                eventClone->generated = true;
                                m_processedDynamicEvents.push_front(std::shared_ptr<Event::DynamicEvent>(eventClone));
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
                                m_processedDynamicEvents.push_front(std::shared_ptr<Event::DynamicEvent>(eventClone));
                            }
                        }
                    }
    }
    void Level::parseMoveTrackData()
    {
        for (const auto& event : m_processedDynamicEvents)
        {
            const auto mt = std::dynamic_pointer_cast<Event::Track::MoveTrack>(event);
            if (mt == nullptr)
                continue;
            const size_t b = rel2absIndex(mt->floor, mt->startTile),
                         e = std::min(tiles.size() - 1, rel2absIndex(mt->floor, mt->endTile));
            for (size_t i = b; i <= e; i++)
            {
                auto& d = tiles[i].moveTrackDatas;
                // clang-format off
                d.emplace_back(mt->floor, mt->angleOffset, mt->beat, mt->seconds, mt->startTile, mt->endTile,
                               mt->duration,
                               mt->positionOffset, 114514, 114514,
                               mt->rotationOffset, 114514,
                               mt->scale, 114514, 114514,
                               mt->opacity, 114514,
                               mt->ease);
                // clang-format on
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
    }
    void Level::updateTileColorInfo(const Event::Track::RecolorTrack* const recolorTrack)
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
            tiles[i].trackColorType.c = recolorTrack->trackColorType;
            tiles[i].trackStyle.c = recolorTrack->trackStyle;
            tiles[i].trackColorPulse.c = recolorTrack->trackColorPulse;
            tiles[i].trackPulseLength.c = recolorTrack->trackPulseLength;
            tiles[i].trackColorAnimDuration.c = recolorTrack->trackColorAnimDuration;
        }
    }

    void Level::updateTileColor(const double seconds, const size_t i)
    {
        auto& tile = tiles[i];
        double x = seconds;
        if (const double y = tile.trackColorAnimDuration.c; y == 0)
            x = 0;
        else
        {
            x /= y;
            if (tile.trackColorPulse.c == TrackColorPulse::Forward)
                x -= static_cast<double>(i) * 1.0 / tile.trackPulseLength.c;
            else if (tile.trackColorPulse.c == TrackColorPulse::Backward)
                x += static_cast<double>(i) * 1.0 / tile.trackPulseLength.c;
            x = positiveRemainder(x, 1.0);
        }

        switch (tile.trackColorType.c)
        {
        case TrackColorType::Single:
            tile.color = tile.trackColor.c;
            break;
        case TrackColorType::Stripes:
            tile.color = i % 2 == 0 ? tile.trackColor.c : tile.secondaryTrackColor.c;
            break;
        case TrackColorType::Glow:
            {
                if (x > 0.5)
                    x = 1 - x;
                const uint8_t a = static_cast<uint8_t>((x > 0.5 ? 1 - x : x) * 2 * 255), b = 255 - a;
                tile.color = tile.trackColor.c * Color(a, a, a, 255) + tile.secondaryTrackColor.c * Color(b, b, b, 255);
                break;
            }

        case TrackColorType::Blink:
            {
                const uint8_t a = static_cast<uint8_t>(x * 255), b = 255 - a;
                tile.color = tile.trackColor.c * Color(a, a, a, 255) + tile.secondaryTrackColor.c * Color(b, b, b, 255);
                break;
            }
        case TrackColorType::Switch:
            {
                tile.color = x > 0.5 ? tile.secondaryTrackColor.c : tile.trackColor.c;
                break;
            }
        case TrackColorType::Rainbow: // TODO
            {
                auto [h, s, v] = tile.trackColor.c.toHSV();
                h += x * 360;
                h = positiveRemainder(h, 360);
                tile.color = Color::fromHSV(h, s, v);
                tile.color.a = tile.trackColor.c.a;
                break;
            }
        case TrackColorType::Volume: // TODO
            {
                tile.color = static_cast<int>(x * 4 /*?*/) % 2 == 0 ? tile.trackColor.c : tile.secondaryTrackColor.c;
            }
        default:
            tile.color = tile.trackColor.c;
        }
    }
    void Level::updateTilePos(const double seconds, const size_t i)
    {
        for (auto& tile = tiles[i]; const auto& data : tile.moveTrackDatas) // FIXME
        {
            if (seconds < data.seconds)
                break;
            const double bpm = getBpmForDynamicEvent(data.floor, data.angleOffset), spb = bpm2crotchet(bpm);
            auto calcX = [&seconds, &data, &spb](const double endSec)
            { return data.duration != 0.0 ? (std::min(seconds, endSec) - data.seconds) / spb / data.duration : 1.0; };
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
} // namespace AdoCpp
