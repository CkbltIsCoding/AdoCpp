#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <imgui.h>

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
	float inputOffset = -90;

	std::array<float, 60> arrFps;
	float avgFps, minFps, maxFps;

	sf::ContextSettings settings;
	sf::View view;
	sf::Vector2f zoom;
	sf::Font font;
	sf::Text textFps{ font };

	sf::Music music;

	AdoCpp::Level level;
	AdoCpp::Difficulty difficulty = AdoCpp::Difficulty::Strict;
	std::filesystem::path levelPath, musicPath;

	TileSystem tileSystem;

	using S = sf::Keyboard::Scan;
	std::vector<sf::Keyboard::Scan> keyLimiter = {
		S::LControl, S::CapsLock, S::Tab, S::Num1, S::Num2, S::E, S::Space, S::C,
		S::Period, S::A, S::P, S::Equal, S::Backspace, S::Backslash, S::Enter, S::Down
	};
	// std::vector<sf::Keyboard::Scan> keyLimiter = {
	// 	S::LControl, S::CapsLock, S::Tab, S::Q, S::W, S::E, S::C, S::Space,
	// 	S::A, S::Period, S::P, S::LBracket, S::RBracket, S::Backslash, S::Enter, S::Down
	// };

	std::vector<State*> states;
	std::optional<size_t> activeTileIndex;
	bool autoplay = false;
	bool fullscreen = false;

	sf::Clock deltaClock;
};
