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
    class LevelNotParsedException : public std::exception
    {
    public:
        LevelNotParsedException() {}
        const char* what() const throw ()
        {
            return "LevelNotParsedException: AdoCpp::Level is not parsed";
        }
    };

    enum class Difficulty
    {
        Lenient, Normal, Strict
    };
    enum class HitMargin
    {
        Perfect,
        LatePerfect, EarlyPerfect,
        VeryLate, VeryEarly,
        TooLate, TooEarly
    };

    struct Tile
    {
        double angle = 0;
        bool orbit = true;
        double beat = 0;
        double oRotation = 0, rotation = 0;
        double opacity = 100;
        bool stickToFloors = false;
        Point editorPos, oPos, pos;
        Point oScale = { 100, 100 }, scale = { 100, 100 };
        std::string oTrackColor = "debb7b", trackColor = "debb7b";
        std::string oSecondaryTrackColor = "ffffff", secondaryTrackColor = "ffffff";
        TrackStyle oTrackStyle = TrackStyle::Standard, trackStyle = TrackStyle::Standard;
        std::vector<Event::Event*> events;
        Tile(double angle)
            : angle(angle)
        {
        }
    };

    struct Settings
    {
        int version;
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
        std::string backgroundColor = "000000";
        bool stickToFloors = false;
    };


    class Level
    {
    public:
        Level();
        ~Level();

        void clear();

        void defaultLevel();

        /**
         * @brief 把json数据导入到关卡
         * @param data json数据
         */
        void fromJson(const rapidjson::Document& data);
        /**
         * @brief 把文件导入到关卡 (以UTF-8 BOM编码)
         * @param ifs ifstream类型文件流
         */
        void fromFile(std::ifstream& ifs);
        /**
         * @brief 把文件导入到关卡 (以UTF-8 BOM编码)
         * @param path 文件路径
         */
        void fromFile(std::string path);

        /**
         * @brief 解析关卡
         * 解析关卡。
         * 调用此方法后，才能调用更多方法（如update()）。
         */
        void parse();

        /**
         * @brief 更新关卡
         * 更新关卡。
         */
        void update();
        /**
         * @brief 更新关卡
         * @param beat 拍数
         * 更新关卡，使关卡变成在指定拍子的样子。
         */
        void update(double beat);

        /**
         * @brief 使相对于index的下标变成绝对下标
         * @param index 砖块下标
         * @param relativeIndex 相对于index的下标
         * 其中，relativeIndex.second必须为"Start"或"End"或"ThisTile"。
         * @return 绝对下标
         * @note
         * 为了应付一些带有相对下标（如startTile, endTile, relativeTo）的事件，
         * 此方法将相对的下标变成绝对的下标。
         */
        size_t rel2absIndex(size_t index, RelativeIndex relativeIndex) const;

        /**
         * @brief 获取两个行星的坐标
         * @param index 砖块下标
         * @param beat 拍子
         * @return 两个行星的坐标
         */
        std::pair<Point, Point> getPlanetsPos(size_t index, double beat);

        bool isFirePlanetStatic(size_t index);

        /**
         * @brief 获取在beat拍子时刻行星所在的砖块下标
         * @param beat 拍数
         * @return 砖块下标
         */
        size_t getTileIndexByBeat(double beat);

        /**
         * @brief 获取在beat拍子时刻的BPM
         * @param beat 拍数
         * @return BPM
         */
        double getBpmByBeat(double beat);

        /**
         * @brief 将beat转换为timer
         * @param 拍子
         * @return timer 计时器（以ms为单位）
         */
        double beat2timer(double beat);

        /**
         * @brief 将timer转换为beat
         * @param timer 计时器（以ms为单位）
         * @return 拍子
         */
        double timer2beat(double timer);

        double getAngle(size_t index);

        struct CameraInfo
        {
            Point position;
            double rotation;
            double zoom;
        };

        /**
         * @brief 获取在beat拍子时刻摄像机的位置、旋转、缩放信息
         * @param beat 拍子
         * @return 摄像机的位置、旋转、缩放信息
         */
        CameraInfo getCameraInfo(double beat);

        /**
         * @brief 获取 timing
         * @param index 砖块下标
         * @param timer 计时器（以ms为单位）
         * @return timing
         */
        double getTiming(size_t index, double timer);

        /**
         * @brief 获取判定
         * @param index 砖块下标
         * @param timer 计时器（以ms为单位）
         * @param difficulty 难度
         * @return 判定
         */
        HitMargin getHitMargin(size_t index, double timer, Difficulty difficulty);



        bool isParsed() const;

        Settings settings;
        std::vector<Tile> tiles;
        std::vector<Event::Event*> events;
    protected:
        /**
         * @brief 关卡是否解析过
         * 关卡是否解析过
         */
        bool parsed = false;
    private:
        std::vector<Event::BeatEvent*> m_processedBeatEvents;
        std::vector<Event::GamePlay::SetSpeed> m_setSpeeds;
        std::vector<Event::Visual::MoveCamera> m_moveCameras;
        std::vector<Point> m_moveCameraPlayerPosVec;
        std::vector<Point> m_moveCameraPlayerPosVec2;
        Point f(double beat);
    };
}

