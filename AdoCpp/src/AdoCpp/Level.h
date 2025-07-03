#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <vector>

#include "DynamicValue.h"
#include "Event.h"
#include "Math/Vector2.h"
#include "Utils.h"
#include "rapidjson/error/en.h"

namespace AdoCpp
{
    /**
     * @brief An exception.
     */
    class LevelNotParsedException final : public std::exception
    {
    public:
        LevelNotParsedException() = default;
        [[nodiscard]] const char* what() const noexcept override
        {
            return "AdoCpp::LevelNotParsedException: AdoCpp::Level is not parsed";
        }
    };

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

    class LevelPathAngleDataNotFoundException final : public std::exception
    {
    public:
        LevelPathAngleDataNotFoundException() = default;
        [[nodiscard]] const char* what() const noexcept override
        {
            return "LevelPathAngleDataNotFoundException: The json data doesn't have AngleData or PathData";
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
         * @brief The tile's color.
         */
        DynamicValue<Color> trackColor{Color(0xdebb7b)};
        /**
         * @brief The tile's secondary color.
         */
        DynamicValue<Color> secondaryTrackColor{Color(0xffffff)};
        /**
         * @brief The tile's style.
         */
        DynamicValue<TrackStyle> trackStyle{TrackStyle::Standard};
        /**
         * @brief The event ptrs of the tile.
         */
        std::vector<Event::Event*> events;

        size_t trackAnimationFloor = 0;
        TrackAnimation trackAnimation = TrackAnimation::None;
        double beatsAhead = 0;
        TrackDisappearAnimation trackDisappearAnimation = TrackDisappearAnimation::None;
        double beatsBehind = 0;

        struct MoveTrackData
        {
            size_t floor;
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
        std::string hitsound;
        double hitsoundVolume = 100;
        double countdownTicks = 0;
        TrackColorType trackColorType = TrackColorType::Single;
        Color trackColor = Color("debb7b");
        Color secondaryTrackColor = Color("debb7b");
        double trackColorAnimDuration = 0;
        TrackColorPulse trackColorPulse = TrackColorPulse::None;
        double trackPulseLength = 1;
        TrackStyle trackStyle = TrackStyle::Standard;
        TrackAnimation trackAnimation;
        double beatsAhead = 0;
        TrackDisappearAnimation trackDisappearAnimation;
        double beatsBehind = 0;
        Color backgroundColor = Color("000000");
        bool stickToFloors = false;
        double unscaledSize = 100;
        RelativeToCamera relativeTo = RelativeToCamera::Player;
        Vector2lf position;
        double rotation = 0;
        double zoom = 100;

        /**
         * Apply the settings to the tile.
         * @param tile The tile.
         */
        void apply(Tile& tile) const
        {
            tile.trackColor.o = trackColor, tile.secondaryTrackColor.o = secondaryTrackColor,
            tile.trackStyle.o = trackStyle, tile.editorPos = tile.pos.o = {0, 0}, tile.stickToFloors = stickToFloors,
            tile.trackAnimationFloor = 0, tile.trackAnimation = trackAnimation, tile.beatsAhead = beatsAhead,
            tile.trackDisappearAnimation = trackDisappearAnimation, tile.beatsBehind = beatsBehind;
        }
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

        /**
         * @brief Default destructor.
         */
        ~Level();

        /**
         * @brief Clear the level class.
         */
        void clear();

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
         * @param ifs ifstream.
         */
        void fromFile(std::ifstream& ifs);
        /**
         * @brief Import a file into the level (encoded in UTF-8 BOM).
         * @param path The path to the file.
         */
        void fromFile(const std::filesystem::path& path);

        /**
         * @brief Parse the level.
         */
        void parse();

        /**
         * @brief Update the level.
         */
        void update();
        /**
         * @brief Update the level.
         * @param seconds The seconds.
         */
        void update(const double& seconds);

        /**
         * @brief Convert baseIndex + relativeIndex into absolute index.
         * @param baseIndex The base index.
         * @param relativeIndex The index relative to the base index.
         * @return The absolute index.
         */
        [[nodiscard]] size_t rel2absIndex(const size_t& baseIndex, const RelativeIndex& relativeIndex) const;

        /**
         * @brief Get the included angle between the two planets.
         * @param index The index of the tile.
         * @param seconds The seconds.
         * @return The position of the two planets.
         */
        [[nodiscard]] double getPlanetsDir(const size_t& index, const double& seconds) const;

        /**
         * @brief Get the position of the two planets.
         * @param index The index of the tile.
         * @param seconds The seconds.
         * @return The position of the two planets.
         */
        [[nodiscard]] std::pair<Vector2lf, Vector2lf> getPlanetsPos(const size_t& index, const double& seconds) const;

        [[nodiscard]] static bool isFirePlanetStatic(size_t index);

        /**
         * @brief Get the index of the tile that one of the planets lands on.
         * @param beat The beat.
         * @return The index of the tile.
         */
        [[nodiscard]] size_t getTileIndexByBeat(const double& beat) const;

        /**
         * @brief Get the index of the tile that one of the planets lands on.
         * @param seconds The seconds.
         * @return The index of the tile.
         */
        [[nodiscard]] size_t getTileIndexBySeconds(const double& seconds) const;

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
        [[nodiscard]] double getBpmByBeat(const double& beat) const;
        /**
         * @brief Get the bpm.
         * @param beat The beat.
         * @return The bpm.
         */
        [[nodiscard]] double getBpmBySeconds(const double& seconds) const;

        /**
         * @brief Get the bpm.
         * @param beat The beat.
         * @return The bpm.
         */
        [[nodiscard]] double getBpmExcludingBeat(const double& beat) const;

        /**
         * @brief Convert beat to seconds.
         * @param beat The beat.
         * @return The time in seconds.
         */
        [[nodiscard]] double beat2seconds(const double& beat) const;

        /**
         * @brief Convert seconds to beat.
         * @param seconds The time in seconds.
         * @return The beat.
         */
        [[nodiscard]] double seconds2beat(double seconds) const;

        [[nodiscard]] double getAngle(const size_t& index) const;

        /**
         * @brief Get the timing.
         * @param index The index of the tile.
         * @param seconds The time in seconds.
         * @return The timing.
         */
        [[nodiscard]] double getTiming(const size_t& index, const double& seconds) const;

        struct TimingBoundary
        {
            double perfect;
            double lateEarlyPerfect;
            double veryLateEarly;
        };

        [[nodiscard]] TimingBoundary getTimingBoundary(const size_t& index, const Difficulty& difficulty) const;

        /**
         * @brief Get the hit margin.
         * @param index The index of the tile.
         * @param seconds The time in seconds.
         * @param difficulty The difficulty.
         * @return The hit margin.
         */
        [[nodiscard]] HitMargin getHitMargin(const size_t& index, const double& seconds,
                                             const Difficulty& difficulty) const;

        /**
         * @brief Get whether the level has been parsed.
         * @return Whether the level has been parsed.
         */
        [[nodiscard]] bool isParsed() const;

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
        void updateCamera(const double& seconds, const size_t& floor);

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

    protected:
        /**
         * @brief Whether the level has been parsed.
         */
        bool parsed = false;

    private:
        struct MoveCameraData
        {
            size_t floor;
            double beat;
            double seconds;
            double duration;
            std::optional<RelativeToCamera> relativeTo;
            bool duplicatedRelPlayer;
            double relEndSec;
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
