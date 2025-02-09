#include <Windows.h>
#include <imgui-SFML.h>

#include "Game.h"
#include "AssetManager.h"
#include "State.h"
#include <State/Charting.h>
#include <IconsFontAwesome6.h>

Game::Game() :
	windowSize(800, 600),
	running(true),
	FPS(120),
	//levelPath("Levels/Multi_arm/main.adofai"),
	//levelPath("Levels/HELLO_BPM_2024/2024.adofai"),
	tileSystem(level)
{
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, ".UTF-8");

	settings.antiAliasingLevel = 8;
	window = sf::RenderWindow(sf::VideoMode(windowSize), "A dance of C++", sf::Style::Default, sf::State::Windowed, settings);
	window.setFramerateLimit(120);
	AssetManager mgr;
	font = AssetManager::GetFont("SourceHanSansSC.otf");

	if (!ImGui::SFML::Init(window, false))
	{
		std::cerr << "Error! ImGui::SFML::Init() returns false.";
		exit(-1);
	}
	ImGuiIO& io = ImGui::GetIO();
	//io.FontGlobalScale = 2.f;

	io.Fonts->Clear();

	const float fontSize = 24.f, iconSize = fontSize * 2 / 3.f, fontScale = 1.f;
	io.Fonts->AddFontFromFileTTF(
		"assets/font/Roboto-Regular.ttf", fontSize * fontScale
	);
	static const ImWchar iconRange[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig iconConfig, chineseConfig;
	iconConfig.MergeMode = chineseConfig.MergeMode = true,
		iconConfig.PixelSnapH = true,
		iconConfig.GlyphMinAdvanceX = iconSize;
	io.Fonts->AddFontFromFileTTF(
		"assets/font/fa-solid-900.ttf", iconSize * fontScale, &iconConfig, iconRange);
	io.Fonts->AddFontFromFileTTF(
		"assets/font/SourceHanSansSC.otf", fontSize * fontScale, &chineseConfig,
		io.Fonts->GetGlyphRangesChineseFull());

	if (!ImGui::SFML::UpdateFontTexture())
	{
		std::cerr << "Error! ImGui::SFML::UpdateFontTexture() returns false.";
		exit(-1);
	}

	level.defaultLevel();
	changeState(StateCharting::instance());
}

void Game::run()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime, now;
	auto start = std::chrono::high_resolution_clock().now();
	while (window.isOpen())
	{
		now = std::chrono::high_resolution_clock().now();
		std::chrono::duration<double, std::milli> _gameTime = now - start;
		std::chrono::duration<double, std::milli> _deltaTime = now - lastFrameTime;
		lastFrameTime = now;

		gameTime = _gameTime.count();
		deltaTime = _deltaTime.count();

		fps = 1000 / deltaTime, avgFps = 0, minFps = INT_MAX, maxFps = 0;
		for (size_t i = 0; i < arrFps.size(); i++)
		{
			if (i != arrFps.size() - 1) arrFps[i] = arrFps[i + 1];
			else arrFps[i] = fps;
			avgFps += arrFps[i];
			minFps = std::min(minFps, arrFps[i]);
			maxFps = std::max(maxFps, arrFps[i]);
		}
		avgFps /= arrFps.size();

		handleEvent();
		update();
		render();
	}
	ImGui::SFML::Shutdown();
}

void Game::changeState(State* state)
{
	if (!states.empty())
	{
		states.back()->cleanup();
		states.pop_back();
	}
	states.push_back(state);
	states.back()->init(this);
}

void Game::pushState(State* state)
{
	if (!states.empty())
		states.back()->pause();
	states.push_back(state);
	states.back()->init(this);
}

void Game::popState()
{
	if (!states.empty()) {
		states.back()->cleanup();
		states.pop_back();
	}
	if (!states.empty())
		states.back()->resume();
}

void Game::handleEvent()
{
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			window.close();
		if (const auto* resized = event->getIf<sf::Event::Resized>())
			windowSize = resized->size;
		ImGui::SFML::ProcessEvent(window, *event);
		states.back()->handleEvent(*event);
	}
}

void Game::update()
{
	ImGui::SFML::Update(window, sf::milliseconds((int32_t)deltaTime));
	states.back()->update();
}

void Game::render()
{
	window.clear(sf::Color(20, 20, 20));
	states.back()->render();
	window.display();
}
