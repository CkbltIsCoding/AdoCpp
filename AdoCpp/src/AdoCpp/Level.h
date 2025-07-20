#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <vector>

#include "DynamicValue.h"
#include "Event.h"
#include "Math/Vector2.h"
#include "Utils.h"

namespace AdoCpp
{
    class LevelCouldNotOpenFileException final : public std::exception
    {
    public:
        LevelCouldNotOpenFileException() = default;
        [[nodiscard]] const char* what() const noexcept override
        {
            return "LevelCouldNotOpenFileException: could not open file";
        }
    };

    class LevelJsonException final : public std::exception
    {
    private:
        rapidjson::ParseErrorCode m_code;

    public:
        explicit LevelJsonException(const rapidjson::ParseErrorCode code) { m_code = code; }
        [[nodiscard]] const char* what() const noexcept override
        {
            const auto msg = new char[256];
            strcpy_s(msg, 256, "LevelCouldNotOpenFileException: could not open file");
            strcat_s(msg, 256, rapidjson::GetParseError_En(m_code));
            return msg;
        }
    };

    /**
     * @brief Adofai's difficulty.
     */
    enum class Difficulty
    {
        Lenient,
        Normal,
        Strict
    };

    /**
     * @brief Adofai's hit margin.
     */
    enum class HitMargin
    {
        Perfect,
        LatePerfect,
        EarlyPerfect,
        VeryLate,
        VeryEarly,
        TooLate,
        TooEarly
    };

    /**
     * @brief Tile struct used for storing tile datas.
     */
    struct Tile
    {
        /**
         * @brief The tile's angle.
         */
        Angle angle{};
        /**
         * @brief The orbit of the planets when one of them lands on the tile.
         */
        Orbit orbit = Clockwise;
        /**
         * @brief The tile's beat.
         */
        double beat = 0;
        /**
         * @brief The tile's seconds.
         */
        double seconds = 0;
        /**
         * @brief The current opacity of the tile.
         */
        double opacity = 100;
        /**
         * @brief Whether the planets will stick to this tile.
         */
        bool stickToFloors = false;

        /**
         * @brief The position of the tile in editor.
         */
        Vector2lf editorPos;
        /**
         * @brief The position of the tile.
         */
        DynamicValue<Vector2lf> pos{};
        /**
         * @brief The scale of the tile.
         */
        DynamicValue<Vector2lf> scale{{100, 100}};
        /**
         * @brief The rotation of the tile.
         */
        DynamicValue<double> rotation{};

        /**
         *
         */
        DynamicValue<TrackColorType> trackColorType;
        /**
         * @brief The tile's color.
         */
        DynamicValue<Color> trackColor{Color(0xdebb7b)};
        /**
         * @brief The tile's secondary color.
         */
        DynamicValue<Color> secondaryTrackColor{Color(0xffffff)};
        /**
         *
         */
        DynamicValue<double> trackColorAnimDuration;
        /**
         * @brief The tile's style.
         */
        DynamicValue<TrackStyle> trackStyle{TrackStyle::Standard};
        /**
         *
         */
        DynamicValue<TrackColorPulse> trackColorPulse{TrackColorPulse::None};
        /**
         *
         */
        DynamicValue<uint32_t> trackPulseLength{10};
        /**
         *
         */
        Color color;

        /**
         * @brief The event ptrs of the tile.
         */
        std::vector<Event::Event*> events;

        size_t trackAnimationFloor = 0;
        TrackAnimation trackAnimation = TrackAnimation::None;
        double beatsAhead = 0;
        TrackDisappearAnimation trackDisappearAnimation = TrackDisappearAnimation::None;
        double beatsBehind = 0;

        Hitsound hitsound = Hitsound::Kick;
        double hitsoundVolume = 100;
        Hitsound midspinHitsound = Hitsound::Kick;
        double midspinHitsoundVolume = 100;

        struct MoveTrackData
        {
            size_t floor;
            double angleOffset;
            double beat;
            double seconds;
            RelativeIndex startTile;
            RelativeIndex endTile;
            double duration;
            OptionalPoint positionOffset;
            double xEndSec;
            double yEndSec;
            std::optional<double> rotationOffset;
            double rotEndSec;
            OptionalPoint scale;
            double scXEndSec;
            double scYEndSec;
            std::optional<double> opacity;
            double opEndSec;
            Easing ease;
        };
        std::vector<MoveTrackData> moveTrackDatas;

        /**
         * @brief Construct a tile.
         * @param angle The angle of the tile.
         */
        explicit Tile(const double angle) : angle(degrees(angle)) {}
    };

    /**
     * @brief Settings struct.
     */
    struct Settings
    {
        int version = 114514;
        std::string artist;
        std::string song;
        std::string author;
        bool separateCountdownTime = false;
        std::string songFilename;
        double bpm = 100;
        double volume = 100;
        double offset = 0;
        double pitch = 100;
        Hitsound hitsound = Hitsound::Kick;
        double hitsoundVolume = 100;
        double countdownTicks = 0;
        TrackColorType trackColorType = TrackColorType::Single;
        Color trackColor = Color("debb7b");
        Color secondaryTrackColor = Color("debb7b");
        double trackColorAnimDuration = 0;
        TrackColorPulse trackColorPulse = TrackColorPulse::None;
        uint32_t trackPulseLength = 10;
        TrackStyle trackStyle = TrackStyle::Standard;
        TrackAnimation trackAnimation = TrackAnimation::None;
        double beatsAhead = 0;
        TrackDisappearAnimation trackDisappearAnimation = TrackDisappearAnimation::None;
        double beatsBehind = 0;
        Color backgroundColor = Color("000000");
        bool stickToFloors = false;
        double unscaledSize = 100;
        RelativeToCamera relativeTo = RelativeToCamera::Player;
        Vector2lf position;
        double rotation = 0;
        double zoom = 100;

        Settings() = default;
        explicit Settings(const rapidjson::Value& jsonSettings);

        [[nodiscard]] static Settings fromJson(const rapidjson::Value& jsonSettings);
        [[nodiscard]] rapidjson::Value intoJson(rapidjson::Document::AllocatorType& alloc) const;
        [[nodiscard]] rapidjson::Document intoJson() const;

        /**
         * Apply the settings to the tile.
         * @param tile The tile.
         */
        void apply(Tile& tile) const;
    };

    /**
     * @brief Planet struct.
     */
    struct Planet
    {
        Angle angle;
        Vector2lf position;
        double radius{};
    };

    /**
     * @brief Planets struct.
     */
    struct Planets
    {
        Planet fire;
        Planet ice;
        Planet third;
    };

    /**
     * @brief Level class.
     */
    class Level
    {
    public:
        /**
         * Create a new empty Level object.
         * @brief Default constructor.
         */
        Level() = default;
        /**
         * Create a new Level object from json data.
         * @brief Constructor.
         */
        explicit Level(const rapidjson::Document& document);
        /**
         * Create a new Level object from a file encoded in UTF-8 BOM.
         * @brief Constructor.
         */
        explicit Level(std::ifstream& ifs);
        /**
         * Create a new Level object from a file encoded in UTF-8 BOM.
         * @brief Constructor.
         */
        explicit Level(const std::filesystem::path& path);

        Level(const Level&) = delete;

        /**
         * @brief Default destructor.
         */
        ~Level();

        /**
         * @brief Clear the level class.
         */
        void clear();

        void free() const;

        /**
         * @brief Generate the default level.
         * (parsed & updated)
         */
        void defaultLevel();

        /**
         * @brief Import json data into the level.
         * @param document Json data.
         */
        void fromJson(const rapidjson::Document& document);

        /**
         * @brief Import a file into the level (encoded in UTF-8 BOM).
         * @param ifs The input file stream.
         */
        void fromFile(std::ifstream& ifs);
        /**
         * @brief Import a file into the level (encoded in UTF-8 BOM).
         * @param path The path to the file.
         */
        void fromFile(const std::filesystem::path& path);

        [[nodiscard]] rapidjson::Value intoJson(rapidjson::Document::AllocatorType& alloc) const;
        [[nodiscard]] rapidjson::Document intoJson() const;

        /**
         * @brief Parse the level.
         */
        void parse(bool force = false);

        /**
         * @brief Update the level.
         */
        void update();
        /**
         * @brief Update the level.
         * @param seconds The seconds.
         */
        void update(double seconds);

        /**
         * @brief Insert the tile.
         * @param floor The index.
         * @param tile The tile.
         */
        void insertTile(size_t floor, const Tile& tile);
        /**
         * @brief Insert the tile.
         * @param floor The index.
         * @param angle The angle.
         */
        void insertTile(size_t floor, double angle);
        /**
         * Change the tile's angle into a new one.
         * @param floor The floor.
         * @param angle The new angle.
         */
        void changeTileAngle(size_t floor, double angle);

        /**
         * @brief Erase the tiles.
         * @param pos The position.
         * @param n The number.
         */
        void eraseTile(size_t pos, size_t n = -1ull);

        /**
         * @brief Push back the tile.
         * @param tile The tile.
         */
        void pushBackTile(const Tile& tile);
        /**
         * @brief Push back the tile.
         * @param angle The angle.
         */
        void pushBackTile(double angle);

        /**
         * @brief Pop back the tile.
         */
        void popBackTile();

        /**
         * @brief Add the event.
         * @param event The event.
         * @param index The index.
         */
        void addEvent(const Event::Event* event, size_t index);
        /**
         * @brief Remove the event.
         * @param floor The floor.
         * @param index The index.
         * @return Whether it is success.
         */
        bool removeEvent(size_t floor, size_t index);

        /**
         * @brief Convert baseIndex + relativeIndex into absolute index.
         * @param baseIndex The base index.
         * @param relativeIndex The index relative to the base index.
         * @return The absolute index.
         */
        [[nodiscard]] size_t rel2absIndex(size_t baseIndex, RelativeIndex relativeIndex) const;

        /**
         * @brief Get the included angle between the two planets.
         * @param floor The index of the tile.
         * @param seconds The seconds.
         * @return The position of the two planets.
         */
        [[nodiscard]] double getPlanetsDir(size_t floor, double seconds) const;

        /**
         * @brief Get the position of the two planets.
         * @param floor The index of the tile.
         * @param seconds The seconds.
         * @return The position of the two planets.
         */
        [[nodiscard]] std::pair<Vector2lf, Vector2lf> getPlanetsPos(size_t floor, double seconds) const;

        [[nodiscard]] static bool isFirePlanetStatic(size_t floor);

        /**
         * @brief Get the index of the tile that one of the planets lands on.
         * @param beat The beat.
         * @return The index of the tile.
         */
        [[nodiscard]] size_t getTileIndexByBeat(double beat) const;

        /**
         * @brief Get the index of the tile that one of the planets lands on.
         * @param seconds The seconds.
         * @return The index of the tile.
         */
        [[nodiscard]] size_t getTileIndexBySeconds(double seconds) const;

        /**
         * @brief Get the bpm.
         * @param func The bool function.
         * @return The bpm.
         */
        [[nodiscard]] double getBpm(const std::function<bool(const Event::GamePlay::SetSpeed&)>& func) const;

        /**
         * @brief Get the bpm.
         * @param beat The beat.
         * @return The bpm.
         */
        [[nodiscard]] double getBpmByBeat(double beat) const;
        /**
         * @brief Get the bpm.
         * @param seconds The seconds.
         * @return The bpm.
         */
        [[nodiscard]] double getBpmBySeconds(double seconds) const;

        /**
         * @brief Get the bpm.
         * @param beat The beat.
         * @return The bpm.
         */
        [[nodiscard]] double getBpmExcludingBeat(double beat) const;

        /**
         * @brief Get the bpm.
         * @param floor The floor.
         * @param angleOffset The angle offset.
         * @return The bpm.
         */
        [[nodiscard]] double getBpmForDynamicEvent(size_t floor, double angleOffset) const;

        /**
         * @brief Convert beat to seconds.
         * @param beat The beat.
         * @return The time in seconds.
         */
        [[nodiscard]] double beat2seconds(double beat) const;

        /**
         * @brief Convert seconds to beat.
         * @param seconds The time in seconds.
         * @return The beat.
         */
        [[nodiscard]] double seconds2beat(double seconds) const;

        [[nodiscard]] double getAngle(size_t floor) const;

        /**
         * @brief Get the timing.
         * @param floor The index of the tile.
         * @param seconds The time in seconds.
         * @return The timing.
         */
        [[nodiscard]] double getTiming(size_t floor, double seconds) const;

        struct TimingBoundary
        {
            double perfect;
            double lateEarlyPerfect;
            double veryLateEarly;
        };

        [[nodiscard]] TimingBoundary getTimingBoundary(size_t floor, Difficulty difficulty) const;

        /**
         * @brief Get the hit margin.
         * @param floor The index of the tile.
         * @param seconds The time in seconds.
         * @param difficulty The difficulty.
         * @return The hit margin.
         */
        [[nodiscard]] HitMargin getHitMargin(size_t floor, double seconds, Difficulty difficulty) const;

        /**
         * @brief Get whether the level has been parsed.
         * @return Whether the level has been parsed.
         */
        [[nodiscard]] bool isParsed() const noexcept;

        struct CameraValue
        {
            Vector2lf position;
            double rotation{};
            double zoom{};
        };

        /**
         * @brief Get the camera info at present.
         * @return The camera info.
         */
        [[nodiscard]] CameraValue cameraValue() const;

        /**
         * @brief Init the camera.
         */
        void initCamera();
        /**
         * @brief Update the camera.
         * @param seconds The seconds.
         * @param floor The floor planet land on.
         */
        void updateCamera(double seconds, size_t floor);

        [[nodiscard]] Settings& getSettings();
        [[nodiscard]] const Settings& getSettings() const;
        [[nodiscard]] const std::vector<Tile>& getTiles() const;
        [[nodiscard]] const std::vector<Event::Event*>& getEvents() const;

    protected:
        /**
         * @brief Whether the level has been parsed.
         */
        bool parsed = false;

        /**
         * @brief The level's settings.
         */
        Settings settings;

        /**
         * @brief The level's tiles.
         */
        std::vector<Tile> tiles;

        /**
         * @brief The level's event pointers.
         */
        std::vector<Event::Event*> events;

    private:
        void parseTiles();
        void parseTileEventsAndSetSpeed();
        void parseDynamicEvents(std::vector<Event::DynamicEvent*>& dynamicEvents,
                                std::vector<std::vector<Event::Modifiers::RepeatEvents*>>& vecRe);
        void parseAnimateTrack();
        void parseRepeatEvents(const std::vector<Event::DynamicEvent*>& dynamicEvents,
                               const std::vector<std::vector<Event::Modifiers::RepeatEvents*>>& vecRe);
        void parseMoveTrackData();

        void updateTileColorInfo(const Event::Track::RecolorTrack* recolorTrack);
        void updateTileColor(double seconds, size_t i);
        void updateTilePos(double seconds, size_t i);

        struct MoveCameraData
        {
            size_t floor;
            double angleOffset;
            double beat;
            double seconds;
            double duration;
            std::optional<RelativeToCamera> relativeTo;
            bool duplicatedRelPlayer;
            double relEndSec;
            Vector2lf playerLastPos;
            OptionalPoint position;
            double xEndSec;
            double yEndSec;
            std::optional<double> rotation;
            double rotEndSec;
            std::optional<double> zoom;
            double zoomEndSec;
            Easing ease;
        };

        std::list<Event::DynamicEvent*> m_processedDynamicEvents;
        std::vector<Event::GamePlay::SetSpeed*> m_setSpeeds;
        std::vector<MoveCameraData> m_moveCameraDatas;

        struct Camera
        {
            Vector2lf position;
            double rotation{};
            double zoom = 100;
            Vector2lf player;
            double lastSeconds = std::numeric_limits<double>::min();
            size_t lastFloor{};
            Vector2lf lastChangedPos;
            size_t lastEventIndex{};
        } m_camera;
    };
} // namespace AdoCpp
