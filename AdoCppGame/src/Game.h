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

	void createWindow();

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
	AdoCpp::Difficulty difficulty = AdoCpp::Difficulty::Strict;
	std::filesystem::path levelPath, musicPath;

	TileSystem tileSystem;

	using S = sf::Keyboard::Scan;
	std::vector<sf::Keyboard::Scan> keyLimiter = {
		S::LControl, S::CapsLock, S::Tab, S::Num1, S::Num2, S::E, S::Space, S::C,
		S::Period, S::A, S::P, S::Equal, S::Backspace, S::Backslash, S::Enter, S::Down
	};
	std::vector<KeyViewerSystem::Key> kvsKeyLimiter = {
		{S::LControl, {0, 1}},
		{S::CapsLock, {1, 1}},
		{S::Tab, {0, 0}},
		{S::Num1, {1, 0}},
		{S::Num2, {2, 0}},
		{S::E, {3, 0}},
		{S::C, {2, 1}},
		{S::Space, {3, 1}},
		{S::A, {4, 1}},
		{S::Period, {5, 1}},
		{S::P, {4, 0}},
		{S::Equal, {5, 0}},
		{S::Backspace, {6, 0}},
		{S::Backslash, {7, 0}},
		{S::Enter, {6, 1}},
		{S::Down, {7, 1}},
	};

	std::vector<State*> states;
	long long activeTileIndex;
	bool autoplay = false;
	bool fullscreen = false;
};
