#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>

#include "Event.h"
#include "Utils.h"

namespace AdoCpp
{
    /**
     * @brief An exception.
     */
    class LevelNotParsedException : public std::exception
    {
    public:
        LevelNotParsedException() {}
        const char* what() const throw ()
        {
            return "LevelNotParsedException: AdoCpp::Level is not parsed";
        }
    };

    /**
     * @brief Adofai's difficulty.
     */
    enum class Difficulty
    {
        Lenient, Normal, Strict
    };
    /**
     * @brief Adofai's hit margin.
     */
    enum class HitMargin
    {
        Perfect,
        LatePerfect, EarlyPerfect,
        VeryLate, VeryEarly,
        TooLate, TooEarly
    };
    
    /**
     * @brief Tile struct used for storing tile datas.
     */
    struct Tile
    {
        /**
         * @brief The tile's angle.
         */
        double angle = 0;
        /**
         * @brief The orbit of the planets when one of them lands on the tile.
         */
        bool orbit = true;
        /**
         * @brief The tile's beat.
         */
        double beat = 0;
        /**
         * @brief The original rotation of the tile.
         */
        double oRotation = 0;
        /**
         * @brief The current rotation of the tile.
         */
        double rotation = 0;
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
        Point editorPos;
        /**
         * @brief The original position of the tile.
         */
        Point oPos;
        /**
         * @brief The current position of the tile.
         */
        Point pos;
        /**
         * @brief The original scale of the tile.
         */
        Point oScale = { 100, 100 };
        /**
         * @brief The current scale of the tile.
         */
        Point scale = { 100, 100 };
        /**
         * @brief The tile's original color.
         */
        Color oTrackColor = "debb7b";
        /**
         * @brief The tile's current color.
         */
        Color trackColor = "debb7b";
        /**
         * @brief The tile's original secondary color.
         */
        Color oSecondaryTrackColor = "ffffff";
        /**
         * @brief The tile's current secondary color.
         */
        Color secondaryTrackColor = "ffffff";
        /**
         * @brief The tile's original style.
         */
        TrackStyle oTrackStyle = TrackStyle::Standard;
        /**
         * @brief The tile's current style.
         */
        TrackStyle trackStyle = TrackStyle::Standard;
        /**
         * @brief The event ptrs of the tile.
         */
        std::vector<Event::Event*> events;
        /**
         * @brief Construct a tile.
         * @param angle The angle of the tile.
         */
        Tile(double angle)
            : angle(angle)
        {
        }
    };

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
        Color trackColor = "debb7b";
        Color secondaryTrackColor = "debb7b";
        double trackColorAnimDuration = 0;
        TrackColorPulse trackColorPulse = TrackColorPulse::None;
        double trackPulseLength = 1;
        TrackStyle trackStyle = TrackStyle::Standard;
        TrackAnimation trackAnimation;
        double beatsAhead = 0;
        TrackAnimation trackDisappearAnimation;
        double beatsBehind = 0;
        Color backgroundColor = "000000";
        bool stickToFloors = false;
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
        Level();
        /**
         * Create a new Level object from json data.
         * @brief Constructor.
         */
        Level(const rapidjson::Document& data);
        /**
         * Create a new Level object from a file encoded in UTF-8 BOM.
         * @brief Constructor.
         */
        Level(std::ifstream& ifs);
        /**
         * Create a new Level object from a file encoded in UTF-8 BOM.
         * @brief Constructor.
         */
        Level(const std::string& path);
        
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
         * @param data Json data.
         */
        void fromJson(const rapidjson::Document& data);
        /**
         * @brief Import a file into the level (encoded in UTF-8 BOM).
         * @param ifs ifstream.
         */
        void fromFile(std::ifstream& ifs);
        /**
         * @brief Import a file into the level (encoded in UTF-8 BOM).
         * @param path The path to the file.
         */
        void fromFile(const std::string& path);

        /**
         * @brief Parse the level.
         * After calling this method, you can call more method
         * such as update().
         */
        void parse();

        /**
         * @brief Update the level.
         */
        void update();
        /**
         * @brief Update the level.
         * @param beat The beat.
         */
        void update(const double& beat);

        /**
         * @brief Make relativeIndex relative to index an absolute index
         * @param index The index.
         * @param relativeIndex The index relative to index.
         * @return The absolute index.
         */
        size_t rel2absIndex(const size_t& index, const RelativeIndex& relativeIndex) const;

        /**
         * @brief Get the position of the two planets.
         * @param index The index of the tile.
         * @param beat The beat.
         * @return The position of the two planets.
         */
        std::pair<Point, Point> getPlanetsPos(const size_t& index, const double& beat) const;

        bool isFirePlanetStatic(size_t index) const;

        /**
         * @brief Get the index of the tile that one of the planets lands on.
         * @param beat The beat.
         * @return The index of the tile.
         */
        size_t getTileIndexByBeat(const double& beat) const;

        /**
         * @brief Get the bpm.
         * @param beat The beat.
         * @return The bpm.
         */
        double getBpmByBeat(const double& beat) const;

        /**
         * @brief Get the bpm.
         * @param beat The beat.
         * @return The bpm.
         */
        double getBpmNotIncludingBeat(const double& beat) const;

        /**
         * @brief Convert beat to timer.
         * @param The beat.
         * @return The time in ms.
         */
        double beat2timer(const double& beat) const;

        /**
         * @brief Convert timer to beat.
         * @param timer The time in ms.
         * @return The beat.
         */
        double timer2beat(double timer) const;

        double getAngle(const size_t& index) const;

        /**
         * @brief CameraInfo struct used for storing the position, rotation and the zoom of the camera.
         */
        struct CameraInfo
        {
            Point position;
            double rotation;
            double zoom;
        };

        /**
         * @brief Get the info of the camera.
         * @param beat The beat.
         * @return The info.
         */
        CameraInfo getCameraInfo(const double& beat) const;

        /**
         * @brief Get the timing.
         * @param index The index of the tile.
         * @param timer The time in ms.
         * @return The timing.
         */
        double getTiming(const size_t& index, const double& timer) const;

        /**
         * @brief Get the hit margin.
         * @param index The index of the tile.
         * @param timer The time in ms.
         * @param difficulty The difficulty.
         * @return The hit margin.
         */
        HitMargin getHitMargin(const size_t& index, const double& timer, const Difficulty& difficulty) const;

        /**
         * @brief Get whether the level has been parsed.
         * @return Whether the level has been parsed.
         */
        bool isParsed() const;

        /**
         * @brief The level's settings.
         */
        Settings settings;
        /**
         * @brief The level's tiles.
         */
        std::vector<Tile> tiles;
        /**
         * @brief The level's event ptrs.
         */
        std::vector<Event::Event*> events;
    protected:
        /**
         * @brief Whether the level has been parsed.
         */
        bool parsed = false;
    private:
        std::vector<Event::BeatEvent*> m_processedBeatEvents;
        std::vector<Event::GamePlay::SetSpeed> m_setSpeeds;
        std::vector<Event::Visual::MoveCamera> m_moveCameras;
        std::vector<Point> m_moveCameraPlayerPosVec;
        std::vector<Point> m_moveCameraPlayerPosVec2;
        Point f(double beat) const;
    };
}

