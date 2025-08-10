#pragma once
#include <AdoCpp.h>
#include <rapidjson/document.h>
#include "SFML/Window.hpp"
struct Config
{
    AdoCpp::Difficulty difficulty = AdoCpp::Difficulty::Strict;
    float inputOffset = -200;
    uint32_t fpsLimit = 0;
    bool syncWithMusic = false;
    bool disableAnimationTrack = false;
    using enum sf::Keyboard::Scan;
    std::vector<sf::Keyboard::Scan> keyLimiter = {
        LControl, CapsLock, Tab, Q, W, E, C, Space,
        A, Period, P, LBracket, RBracket, Backslash, Enter, Down
    };
    // std::vector<sf::Keyboard::Scan> keyLimiter = {
    // 	LControl, CapsLock, Tab, Num1, Num2, E, C, Space,
    // 	A, Period, P, Equal, Backspace, Backslash, Enter, Down
    // };
    // std::vector<sf::Keyboard::Scan> keyLimiter = {Tab, Q, W, E, C, Space, A, Period, P, LBracket, RBracket, Backslash};

    void load();
    void save();
};
