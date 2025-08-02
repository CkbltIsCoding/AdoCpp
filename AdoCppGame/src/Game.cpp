#include <Windows.h>
#include <imgui-SFML.h>

#include "Game.h"
#include "State.h"
#include <State/Charting.h>
#include <IconsFontAwesome6.h>
#include <iostream>
#include <implot.h>

Game::Game() :
	running(true),
	fps(),
	arrFps(),
	avgFps(),
	minFps(),
	maxFps(),
	tileSystem(level)
{
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, ".UTF-8");

	settings.antiAliasingLevel = 8;
	createWindow();
	// FPS = 120;
	// window.setFramerateLimit(FPS);
	//window.setVerticalSyncEnabled(true);
	font = sf::Font("assets/font/SourceHanSansSC.otf");
	if (!ImGui::SFML::Init(window, false))
    {
        std::cerr << "Error! ImGui::SFML::Init() returns false.";
        exit(-1);
    }
    const ImGuiIO& io = ImGui::GetIO();
	//io.FontGlobalScale = 2;

	io.Fonts->Clear();

	const float fontSize = 24, iconSize = fontSize * 2 / 3, fontScale = 1;
	io.Fonts->AddFontFromFileTTF(
		"assets/font/Maplestory OTF Bold.otf", fontSize * fontScale
	);
	static constexpr ImWchar iconRange[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
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

	auto& style = ImGui::GetStyle();
	style.WindowRounding = 10;
	style.ChildRounding = 10;
	style.FrameRounding = 5;
	style.PopupRounding = 10;

    ImPlot::CreateContext();

	level.defaultLevel();
	changeState(StateCharting::instance());
}

void Game::run()
{
	while (true)
	{
		deltaTime = deltaClock.restart();
		fps = 1 / deltaTime.asSeconds(), avgFps = 0, minFps = INT_MAX, maxFps = 0;
		for (size_t i = 0; i < arrFps.size(); i++)
		{
			if (i != arrFps.size() - 1) arrFps[i] = arrFps[i + 1];
			else arrFps[i] = fps;
			avgFps += arrFps[i];
			minFps = std::min(minFps, arrFps[i]);
			maxFps = std::max(maxFps, arrFps[i]);
		}
		avgFps /= static_cast<float>(arrFps.size());

		handleEvent();
		if (!window.isOpen()) break;
		update();
		render();
	}
    ImPlot::DestroyContext();
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
    using enum sf::Keyboard::Key;
	while (const auto event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			window.close();
		if (const auto resized = event->getIf<sf::Event::Resized>())
			windowSize = resized->size;
		if (const auto keyPressed = event->getIf<sf::Event::KeyPressed>())
			if (keyPressed->code == F11)
				fullscreen = !fullscreen, createWindow();
		ImGui::SFML::ProcessEvent(window, *event);
		states.back()->handleEvent(*event);
	}
}

void Game::update()
{
	ImGui::SFML::Update(window, deltaTime);
	states.back()->update();
}

void Game::render()
{
	window.clear(sf::Color(20, 20, 20));
	states.back()->render();
	ImGui::SFML::Render(window);
	window.display();
}

void Game::createWindow()
{
	if (fullscreen)
		windowSize = sf::VideoMode::getFullscreenModes()[0].size,
		window.create(sf::VideoMode::getFullscreenModes()[0], "A dance of C++", sf::Style::None, sf::State::Fullscreen, settings);
	else
		windowSize = { 800, 600 },
		window.create(sf::VideoMode(windowSize), "A dance of C++", sf::Style::Default, sf::State::Windowed, settings);
}
