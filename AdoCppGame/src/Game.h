#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <imgui.h>

#include <AdoCpp.h>
#include "Tile.h"
#include "HitText.h"
#include "HitErrorMeter.h"
#include "KeyViewer.h"

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

	sf::RenderWindow window;
	sf::Vector2u windowSize;
	bool running;
	double gameTime, deltaTime, fps;
	unsigned int FPS;
	double planetRadiusPx = 50;
	double inputOffset = -200;

	std::array<double, 60> arrFps;
	double avgFps, minFps, maxFps;

	sf::ContextSettings settings;
	sf::View view;
	sf::Vector2f zoom;
	sf::Font font;
	sf::Text textFps{ font };

	sf::Music music;

	AdoCpp::Level level;
	AdoCpp::Difficulty difficulty = AdoCpp::Difficulty::Lenient;
	std::filesystem::path levelPath, musicPath;

	TileSystem tileSystem;

	using S = sf::Keyboard::Scan;
	std::vector<sf::Keyboard::Scan> keyLimiter = {
		S::LControl, S::CapsLock, S::Tab, S::Num1, S::Num2, S::E, S::C, S::Space,
		S::A, S::Period, S::P, S::Equal, S::Backspace, S::Backslash, S::Enter, S::Down
	};

	std::vector<State*> states;
	long long activeTileIndex;
	bool autoplay = false;
};
