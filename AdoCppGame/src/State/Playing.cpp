#include "Playing.h"
#include <imgui-SFML.h>
#include <IconsFontAwesome6.h>

StatePlaying StatePlaying::m_statePlaying;

void StatePlaying::init(Game* game)
{
	m_game = game;

	planet1.setFillColor(sf::Color::Red);
	planet2.setFillColor(sf::Color::Blue);
	planet1.setRadius(0.25);
	planet2.setRadius(0.25);
	planet1.setOrigin({ planet1.getRadius(), planet1.getRadius() });
	planet2.setOrigin({ planet2.getRadius(), planet2.getRadius() });

	hitTextSystem.clear();
	hitErrorMeterSystem.setScale({ 4.f, 4.f });
	hitErrorMeterSystem.clear();
	keyViewerSystem = KeyViewerSystem(game->keyLimiter);
	keyViewerSystem.setScale({ 4.f, 4.f });

	keyInputCnt = 0;
	waiting = true;
	if (m_game->activeTileIndex <= 0)
	{
		playerTileIndex = 0;
		timer = beat = -INFINITY;
	}
	else
	{
		playerTileIndex = m_game->activeTileIndex;
		beat = m_game->level.tiles[playerTileIndex].beat;
		timer = m_game->level.beat2timer(beat);
	}
	m_game->window.setKeyRepeatEnabled(false);
}

void StatePlaying::cleanup()
{
	const bool musicPlayable = m_game->music.getDuration().asMilliseconds() != 0;
	if (musicPlayable)
		m_game->music.stop();
	m_game->window.setKeyRepeatEnabled(true);
}

void StatePlaying::pause()
{
	const bool musicPlayable = m_game->music.getDuration().asMilliseconds() != 0;
	if (musicPlayable)
		m_game->music.pause();
	m_game->window.setKeyRepeatEnabled(true);
}

void StatePlaying::resume()
{
	const bool musicPlayable = m_game->music.getDuration().asMilliseconds() != 0;
	if (musicPlayable
		&& m_game->music.getStatus() == sf::Music::Status::Paused)
		m_game->music.play();
	m_game->window.setKeyRepeatEnabled(false);
}

void StatePlaying::handleEvent(sf::Event event)
{
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::F12)
			m_game->autoplay = !m_game->autoplay;
		else if (keyPressed->code == sf::Keyboard::Key::Escape)
			m_game->activeTileIndex = playerTileIndex, m_game->popState();
		for (auto& scan : m_game->keyLimiter)
		{
			if (scan == keyPressed->scancode)
			{
				keyInputCnt++;
				break;
			}
		}
	}
}

void StatePlaying::update()
{
	const bool musicPlayable = m_game->music.getDuration().asMilliseconds() != 0;
	// Time
	if (!waiting)
	{
		if (musicPlayable)
			timer = m_game->music.getPlayingOffset().asMilliseconds() + m_game->inputOffset;
		else
			timer = spareClock.getElapsedTime().asMilliseconds() + m_game->inputOffset + spareClockOffset;
		beat = m_game->level.timer2beat(timer),
			nowTileIndex = m_game->level.getTileIndexByBeat(beat);
	}
	else
		nowTileIndex = 0;

	// Update the level
	if (waiting)
		m_game->level.update();
	else
		m_game->level.update(beat);
	///

	// Judgement
	if (waiting && keyInputCnt > 0)
	{
		// Start the music/timer
		waiting = false;
		if (musicPlayable)
			m_game->music.play();
		else
			spareClock.restart();
		if (m_game->activeTileIndex >= 1)
		{
			const double beginTimer = m_game->level.beat2timer
			(m_game->level.tiles[m_game->activeTileIndex].beat)
				- m_game->inputOffset;

			if (musicPlayable)
				m_game->music.setPlayingOffset(sf::milliseconds(int32_t(std::max(0.0, beginTimer))));
			else
				spareClockOffset = m_game->level.beat2timer
				(m_game->level.tiles[m_game->activeTileIndex].beat) - m_game->inputOffset;

			if (musicPlayable)
				timer = m_game->music.getPlayingOffset().asMilliseconds() + m_game->inputOffset;
			else
				timer = spareClock.getElapsedTime().asMilliseconds() + m_game->inputOffset + spareClockOffset;
			beat = m_game->level.timer2beat(timer),
				nowTileIndex = m_game->level.getTileIndexByBeat(beat);
		}
 		else
		{
			timer = 0, beat = m_game->level.timer2beat(timer);
			if (!musicPlayable)
				spareClockOffset = -m_game->inputOffset;
		}
	}
	if (!waiting && m_game->autoplay)
	{
		keyInputCnt = 0;
		for (size_t i = playerTileIndex; i < nowTileIndex; i++)
		{
			if (m_game->level.tiles[i].angle != 999)
				keyInputCnt++;
		}
	}
	while (!waiting && playerTileIndex < m_game->level.tiles.size() - 1 && keyInputCnt-- > 0)
	{
		playerTileIndex++;
		const double max_bpm = m_game->difficulty == AdoCpp::Difficulty::Lenient
			? 210
			: m_game->difficulty == AdoCpp::Difficulty::Normal
			? 330
			: 500,
			ms = AdoCpp::bpm2mspb(std::min(max_bpm, m_game->level.getBpmByBeat(m_game->level.timer2beat(timer)))),
			p = ms / 6,
			lep = ms / 4,
			vle = ms / 3,
			timing = m_game->level.getTiming(playerTileIndex, timer),
			x = std::min(65.0 / 2, std::max(-65.0 / 2, timing / vle * 65 / 2));
		AdoCpp::HitMargin hitMargin
			= m_game->level.getHitMargin(playerTileIndex, timer, m_game->difficulty);
		if (playerTileIndex == 1 && hitMargin == AdoCpp::HitMargin::TooEarly)
		{
			playerTileIndex--;
			break;
		}

		if (hitMargin == AdoCpp::HitMargin::TooEarly)
		{
			playerTileIndex--;
			AdoCpp::Point pos;
			if (m_game->level.isFirePlanetStatic(playerTileIndex))
				pos = m_game->level.getPlanetsPos(playerTileIndex, beat).second;
			else
				pos = m_game->level.getPlanetsPos(playerTileIndex, beat).first;
			hitTextSystem.addHitText( timer, hitMargin, { (float)pos.first, (float)pos.second });
		}
		else
		{
			if (playerTileIndex != m_game->level.tiles.size() - 1
				&& m_game->level.tiles[playerTileIndex + 1].angle == 999)
				playerTileIndex++;
			hitTextSystem.addHitText(
				timer, hitMargin,
				{ (float)m_game->level.tiles[playerTileIndex].pos.first,
				(float)m_game->level.tiles[playerTileIndex].pos.second });
		}
		hitErrorMeterSystem.addTick(
			timer,
			hitMargin,
			x);
	}
	keyInputCnt = 0;
	while (playerTileIndex < m_game->level.tiles.size() - 1
		&& m_game->level.getHitMargin(playerTileIndex + 1, timer, m_game->difficulty)
		== AdoCpp::HitMargin::TooLate)
	{
		playerTileIndex++;
		if (m_game->level.tiles[playerTileIndex].angle != 999)
		{
			hitTextSystem.addHitText(timer, AdoCpp::HitMargin::TooLate,
				{ (float)m_game->level.tiles[playerTileIndex].pos.first,
				(float)m_game->level.tiles[playerTileIndex].pos.second });
			hitErrorMeterSystem.addTick(
				timer,
				AdoCpp::HitMargin::TooLate,
				65.0 / 2
			);
		}
	}
	///

	// Update planets' positions
	if (!waiting)
	{
		auto p = m_game->level.getPlanetsPos(playerTileIndex, beat);
		planet1.setPosition({ (float)p.first.first, (float)p.first.second });
		planet2.setPosition({ (float)p.second.first, (float)p.second.second });
	}
	else
	{
		auto pos = m_game->level.tiles[playerTileIndex].oPos;
		planet1.setPosition({ (float)pos.first, (float)pos.second });
	}
	///
	
	// Update Systems
	m_game->tileSystem.update();
	hitTextSystem.update(timer);
	hitErrorMeterSystem.update(timer);
	hitErrorMeterSystem.setPosition({ float(m_game->windowSize.x) / 2.f, float(m_game->windowSize.y) - 100.f });
	keyViewerSystem.update(m_game->deltaTime);
	keyViewerSystem.setPosition({ 50.f, float(m_game->windowSize.y) - 500.f });
	///

	// Update the camera
	auto cameraInfo = m_game->level.getCameraInfo(beat);
	m_game->view.setCenter({ float(cameraInfo.position.first), float(cameraInfo.position.second) });
	m_game->view.setRotation(sf::degrees((float)cameraInfo.rotation));
	float w = (float)m_game->windowSize.x, h = (float)m_game->windowSize.y;
	/* aw / (aw + ah) = aw / a(w + h) = w / (w + h)
	   ah / (aw + ah) = ah / a(w + h) = h / (w + h) */
	m_game->zoom = { (float)cameraInfo.zoom / 100, (float)cameraInfo.zoom / 100 };
	m_game->view.setSize(
		{ w / (w + h) * 16 * m_game->zoom.x,
		-h / (w + h) * 16 * m_game->zoom.y});
	///
}

void StatePlaying::render()
{
	// render the world
	m_game->window.setView(m_game->view);
	
	m_game->window.draw(m_game->tileSystem);

	m_game->window.draw(planet1);
	if (!waiting)
		m_game->window.draw(planet2);

	m_game->window.draw(hitTextSystem);

	// render the GUI
	sf::View defaultView = m_game->window.getDefaultView();
	defaultView.setSize(sf::Vector2f(m_game->windowSize));
	defaultView.setCenter(sf::Vector2f(m_game->windowSize) / 2.f);
	m_game->window.setView(defaultView);
	m_game->window.draw(hitErrorMeterSystem);
	m_game->window.draw(keyViewerSystem);

	static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus
		;

	ImGui::SetNextWindowSize(ImVec2(0, 0));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()));
	if (ImGui::Begin("##Left", nullptr, flags))
	{
		std::ostringstream out;
		ImGui::Text("FPS: %.0f avg, %.0f min, %.0f max", m_game->avgFps, m_game->minFps, m_game->maxFps);
		ImGui::Text("Progress: %.2f%%", 100.f * playerTileIndex / (m_game->level.tiles.size() - 1));
		static double bpm, kps;
		bpm = m_game->level.getBpmByBeat(beat);
		kps = bpm / 60
			/ (m_game->level.getAngle(playerTileIndex + (playerTileIndex + 1 == m_game->level.tiles.size() ? 0 : 1)) / 180);
		ImGui::Text("BPM: %.2f", bpm);
		ImGui::Text("KPS: %.2f", kps);
	} ImGui::End();

	ImGui::SFML::Render(m_game->window);
}

