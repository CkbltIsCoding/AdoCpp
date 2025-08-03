#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>

#include <AdoCpp.h>
#include "Tile.h"

class State;

class Game
{
public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game();
    void run();
    void changeState(State* state);
    void pushState(State* state);
    void popState();

    void handleEvent();
    void update();
    void render();

    void createWindow();

    sf::RenderWindow window;
    sf::Vector2u windowSize;
    bool running;
    sf::Time deltaTime;
    float fps;
    unsigned int FPS;
    float planetRadiusPx = 50;
    float inputOffset = -100;
    bool syncWithMusic = false;

    std::array<float, 60> arrFps;
    float avgFps, minFps, maxFps;

    sf::ContextSettings settings;
    sf::View view;
    sf::Vector2f zoom;
    sf::Font font;
    sf::Text textFps{font};

    sf::Music music;

    AdoCpp::Level level;
    AdoCpp::Difficulty difficulty = AdoCpp::Difficulty::Strict;
    std::filesystem::path levelPath, musicPath, origMusicPath;

    TileSystem tileSystem;

    using enum sf::Keyboard::Scan;
    // std::vector<sf::Keyboard::Scan> keyLimiter = {
    // 	LControl, CapsLock, Tab, Num1, Num2, E, C, Space,
    // 	A, Period, P, Equal, Backspace, Backslash, Enter, Down
    // };
    std::vector<sf::Keyboard::Scan> keyLimiter = {
    	LControl, CapsLock, Tab, Q, W, E, C, Space,
    	A, Period, P, LBracket, RBracket, Backslash, Enter, Down
    };
    // std::vector<sf::Keyboard::Scan> keyLimiter = {Tab, Q, W, E, C, Space, A, Period, P, LBracket, RBracket, Backslash};

    std::vector<State*> states;
    std::optional<size_t> activeTileIndex;
    bool autoplay = false;
    bool fullscreen = false;

    sf::Clock deltaClock;
};
