#include "Playing.h"
#include <imgui-SFML.h>
#include <iostream>

StatePlaying StatePlaying::m_statePlaying;

void StatePlaying::init(Game* game)
{
    m_game = game;

    planet1.setFillColor(sf::Color::Red);
    planet2.setFillColor(sf::Color::Blue);
    planet1.setRadius(0.25);
    planet2.setRadius(0.25);
    planet1.setOrigin({planet1.getRadius(), planet1.getRadius()});
    planet2.setOrigin({planet2.getRadius(), planet2.getRadius()});

    hitTextSystem.clear();
    hitErrorMeterSystem.setScale({4, 4});
    hitErrorMeterSystem.clear();
    keyViewerSystem.setKeyLimiter16(game->keyLimiter);
    keyViewerSystem.setScale({6, 6});
    keyViewerSystem.setReleasedColor({255, 100, 100, 63});
    keyViewerSystem.setRainColorByRow({255, 100, 100, 255}, 0);
    keyViewerSystem.setRainColorByRow({255, 255, 255, 191}, 1);

    keyInputCnt = 0;
    waiting = true;
    if (m_game->activeTileIndex.value_or(0) == 0)
    {
        playerTileIndex = 0;
        beat = -m_game->level.settings.countdownTicks;
        seconds = beat * AdoCpp::bpm2crotchet(m_game->level.settings.bpm);
    }
    else
    {
        playerTileIndex = *m_game->activeTileIndex;
        beat = m_game->level.tiles[playerTileIndex].beat;
        seconds = m_game->level.beat2seconds(beat);
    }
    m_game->window.setKeyRepeatEnabled(false);
    isMusicPlayed = false;
}

void StatePlaying::cleanup()
{
    if (musicPlayable())
        m_game->music.stop();
    m_game->window.setKeyRepeatEnabled(true);
}

void StatePlaying::pause()
{
    if (musicPlayable())
        m_game->music.pause();
    m_game->window.setKeyRepeatEnabled(true);
}

void StatePlaying::resume()
{
    if (musicPlayable() && m_game->music.getStatus() == sf::Music::Status::Paused)
        m_game->music.play();
    m_game->window.setKeyRepeatEnabled(false);
}

void StatePlaying::handleEvent(sf::Event event)
{
    if (!ImGui::GetIO().WantCaptureKeyboard)
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
                    keyViewerSystem.press(scan);
                    break;
                }
            }
        }
        else if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
        {
            keyViewerSystem.release(keyReleased->scancode);
        }
    }
}

void StatePlaying::update()
{
    // ReSharper disable CppFunctionalStyleCast
    // Time
    if (waiting && keyInputCnt > 0)
    {
        // Start the music/timer
        waiting = false;
        keyInputCnt--;

        spareClock.restart();
        if (m_game->activeTileIndex.value_or(0) != 0)
        {
            const double beginTimer = m_game->level.beat2seconds(m_game->level.tiles[*m_game->activeTileIndex].beat) -
                m_game->inputOffset / 1000;

            if (musicPlayable()) // FIXME
                m_game->music.setPlayingOffset(sf::seconds(std::max(0.0, beginTimer)));
            else
                spareClockOffset = m_game->level.beat2seconds(m_game->level.tiles[*m_game->activeTileIndex].beat) -
                    m_game->inputOffset / 1000;

            if (musicPlayable())
                seconds = m_game->music.getPlayingOffset().asSeconds() + m_game->inputOffset / 1000;
            else
                seconds = spareClock.getElapsedTime().asSeconds() + m_game->inputOffset / 1000 + spareClockOffset;
            beat = m_game->level.seconds2beat(seconds), nowTileIndex = m_game->level.getTileIndexByBeat(beat);
        }
        else
        {
            seconds =
                (std::min)(0.0,
                           -m_game->level.settings.countdownTicks * AdoCpp::bpm2crotchet(m_game->level.settings.bpm)) +
                m_game->inputOffset / 1000,
            beat = m_game->level.seconds2beat(seconds);
            if (!musicPlayable())
                spareClockOffset = -m_game->inputOffset / 1000;
        }
    }
    if (!waiting)
    {
        if (musicPlayable())
        {
            if (m_game->music.getStatus() == sf::Music::Status::Stopped)
            {
                seconds += spareClock.restart().asSeconds();
                if (!isMusicPlayed && seconds >= m_game->inputOffset / 1000)
                    m_game->music.play(), spareClock.reset(), isMusicPlayed = true;
            }
            else
                seconds = m_game->music.getPlayingOffset().asSeconds() + m_game->inputOffset / 1000;
        }
        else
            seconds = spareClock.getElapsedTime().asSeconds() + m_game->inputOffset / 1000 + spareClockOffset;
        beat = m_game->level.seconds2beat(seconds), nowTileIndex = m_game->level.getTileIndexByBeat(beat);
    }

    // Update the level
    m_game->level.update(seconds);

    // Judgement
    if (!waiting)
    {
        if (m_game->autoplay)
        {
            keyInputCnt = 0;
            for (size_t i = playerTileIndex; i < nowTileIndex; i++)
            {
                if (m_game->level.tiles[i + 1].angle.deg() != 999)
                    keyInputCnt++;
            }
        }
        while (playerTileIndex < m_game->level.tiles.size() - 1 && keyInputCnt-- > 0)
        {
            playerTileIndex++;
            const auto [p, lep, vle] = m_game->level.getTimingBoundary(playerTileIndex, m_game->difficulty);
            const double timing = m_game->level.getTiming(playerTileIndex, seconds),
                         x = std::min(65.0 / 2, std::max(-65.0 / 2, timing / vle * 65.0 / 2.0));
            // std::cout << timing << ' ' << x << std::endl;
            const AdoCpp::HitMargin hitMargin =
                m_game->level.getHitMargin(playerTileIndex, seconds, m_game->difficulty);
            if (hitMargin == AdoCpp::HitMargin::TooEarly)
            {
                playerTileIndex--;
                if (playerTileIndex == 1)
                    break;
                AdoCpp::Vector2lf pos;
                if (AdoCpp::Level::isFirePlanetStatic(playerTileIndex))
                    pos = m_game->level.getPlanetsPos(playerTileIndex, seconds).second;
                else
                    pos = m_game->level.getPlanetsPos(playerTileIndex, seconds).first;
                hitTextSystem.addHitText(seconds, hitMargin, {float(pos.x), float(pos.y)});
            }
            else
            {
                if (playerTileIndex != m_game->level.tiles.size() - 1 &&
                    m_game->level.tiles[playerTileIndex + 1].angle.deg() == 999)
                    playerTileIndex++;
                hitTextSystem.addHitText(seconds, hitMargin,
                                         {float(m_game->level.tiles[playerTileIndex].pos.c.x),
                                          float(m_game->level.tiles[playerTileIndex].pos.c.y)});
            }
            hitErrorMeterSystem.addTick(seconds, hitMargin, x);
        }
        keyInputCnt = 0;
        while (playerTileIndex < m_game->level.tiles.size() - 1 &&
               m_game->level.getHitMargin(playerTileIndex + 1, seconds, m_game->difficulty) ==
                   AdoCpp::HitMargin::TooLate)
        {
            playerTileIndex++;
            if (m_game->level.tiles[playerTileIndex].angle.deg() != 999)
            {
                hitTextSystem.addHitText(seconds, AdoCpp::HitMargin::TooLate,
                                         {static_cast<float>(m_game->level.tiles[playerTileIndex].pos.c.x),
                                          static_cast<float>(m_game->level.tiles[playerTileIndex].pos.c.y)});
                hitErrorMeterSystem.addTick(seconds, AdoCpp::HitMargin::TooLate, 65.0 / 2);
            }
        }
    }

    // Update planets' positions
    if (!waiting)
    {
        const auto [p1pos, p2pos] = m_game->level.getPlanetsPos(playerTileIndex, seconds);
        planet1.setPosition({float(p1pos.x), float(p1pos.y)});
        planet2.setPosition({float(p2pos.x), float(p2pos.y)});
    }
    else
    {
        const auto pos = m_game->level.tiles[playerTileIndex].pos.o;
        planet1.setPosition({float(pos.x), float(pos.y)});
    }

    // Update Systems
    m_game->tileSystem.update();
    hitTextSystem.update(seconds);
    hitErrorMeterSystem.update(seconds);
    hitErrorMeterSystem.setPosition({float(m_game->windowSize.x) / 2, float(m_game->windowSize.y) - 100});
    keyViewerSystem.update();
    keyViewerSystem.setPosition({50.f, float(m_game->windowSize.y) - 500});

    // Update the camera
    m_game->level.updateCamera(seconds, playerTileIndex);
    const auto [pos, rot, zoom] = m_game->level.cameraValue();
    m_game->view.setCenter({float(pos.x), float(pos.y)});
    m_game->view.setRotation(sf::degrees(float(rot)));
    const auto w = float(m_game->windowSize.x), h = float(m_game->windowSize.y);
    /* aw / (aw + ah) = aw / a(w + h) = w / (w + h)
       ah / (aw + ah) = ah / a(w + h) = h / (w + h) */
    m_game->zoom = {float(zoom) / 100, float(zoom) / 100};
    m_game->view.setSize({w / (w + h) * 16 * m_game->zoom.x, -h / (w + h) * 16 * m_game->zoom.y});
    // ReSharper restore CppFunctionalStyleCast
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

    static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()));
    if (ImGui::Begin("LeftText", nullptr, flags))
    {
        ImGui::Text("FPS: %.0f avg, %.0f min, %.0f max", m_game->avgFps, m_game->minFps, m_game->maxFps);
        static float progress, bpm, kps;
        progress = 100 * static_cast<float>(playerTileIndex) / static_cast<float>(m_game->level.tiles.size() - 1);
        bpm = m_game->level.getBpmByBeat(beat);
        kps = bpm / 60 /
            (m_game->level.getAngle(playerTileIndex + (playerTileIndex + 1 == m_game->level.tiles.size() ? 0 : 1)) /
             180);
        ImGui::Text("Progress: %.2f%%", progress);
        ImGui::Text("BPM: %.2f", bpm);
        ImGui::Text("KPS: %.2f", kps);
        ImGui::Text("Floor: %llu", nowTileIndex);
    }
    ImGui::End();
}
