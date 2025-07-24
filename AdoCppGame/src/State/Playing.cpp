#include "Playing.h"
#include <imgui-SFML.h>

StatePlaying StatePlaying::m_statePlaying;

void StatePlaying::init(Game* _game)
{
    game = _game;

    planet1.setFillColor(sf::Color::Red);
    planet2.setFillColor(sf::Color::Blue);
    planet1.setRadius(0.25);
    planet2.setRadius(0.25);
    planet1.setOrigin({planet1.getRadius(), planet1.getRadius()});
    planet2.setOrigin({planet2.getRadius(), planet2.getRadius()});

    hitTextSystem.clear();
    hitErrorMeterSystem.setScale({4, 4});
    hitErrorMeterSystem.clear();
    keyViewerSystem.setKeyLimiterAuto(_game->keyLimiter);
    keyViewerSystem.setScale({6, 6});
    keyViewerSystem.setReleasedColor({255, 100, 100, 63});
    keyViewerSystem.setRainColorByRow({255, 100, 100, 255}, 0);
    keyViewerSystem.setRainColorByRow({255, 255, 255, 191}, 1);

    keyInputCnt = 0;
    waiting = true;
    if (game->activeTileIndex.value_or(0) == 0)
    {
        playerTileIndex = 0;
        const auto& settings = game->level.getSettings();
        beat = -settings.countdownTicks;
        seconds = beat * AdoCpp::bpm2crotchet(settings.bpm);
    }
    else
    {
        playerTileIndex = *game->activeTileIndex;
        beat = game->level.getTiles()[playerTileIndex].beat;
        seconds = game->level.beat2seconds(beat);
    }
    game->window.setKeyRepeatEnabled(false);
    isMusicPlayed = false;
}

void StatePlaying::cleanup()
{
    if (musicPlayable())
        game->music.stop();
    game->window.setKeyRepeatEnabled(true);
}

void StatePlaying::pause()
{
    if (musicPlayable())
        game->music.pause();
    game->window.setKeyRepeatEnabled(true);
}

void StatePlaying::resume()
{
    if (musicPlayable() && game->music.getStatus() == sf::Music::Status::Paused)
        game->music.play();
    game->window.setKeyRepeatEnabled(false);
}

void StatePlaying::handleEvent(const sf::Event event)
{
    using enum sf::Keyboard::Key;
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == F12)
                game->autoplay = !game->autoplay;
            else if (keyPressed->code == Escape)
                game->activeTileIndex = playerTileIndex, game->popState();
            for (const auto& scan : game->keyLimiter)
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
    auto& tiles = game->level.getTiles();
    const auto& settings = game->level.getSettings();

    // ReSharper disable CppFunctionalStyleCast
    // Time
    if (waiting && keyInputCnt > 0)
    {
        // Start the music/timer
        waiting = false;
        keyInputCnt--;

        spareClock.restart();
        if (game->activeTileIndex.value_or(0) != 0)
        {
            const float beginTimer = static_cast<float>(game->level.beat2seconds(tiles[*game->activeTileIndex].beat)) -
                game->inputOffset / 1000;

            if (musicPlayable()) // FIXME
                game->music.setPlayingOffset(sf::seconds(std::max(0.f, beginTimer)));
            else
                spareClockOffset =
                    game->level.beat2seconds(tiles[*game->activeTileIndex].beat) - game->inputOffset / 1000;

            if (musicPlayable())
                seconds = game->music.getPlayingOffset().asSeconds() + game->inputOffset / 1000;
            else
                seconds = spareClock.getElapsedTime().asSeconds() + game->inputOffset / 1000 + spareClockOffset;
            beat = game->level.seconds2beat(seconds), nowTileIndex = game->level.getFloorByBeat(beat);
        }
        else
        {
            seconds = (std::min)(0.0, -settings.countdownTicks * AdoCpp::bpm2crotchet(settings.bpm)) +
                game->inputOffset / 1000,
            beat = game->level.seconds2beat(seconds);
            if (!musicPlayable())
                spareClockOffset = -game->inputOffset / 1000;
        }
    }
    if (!waiting)
    {
        if (musicPlayable())
        {
            if (game->music.getStatus() == sf::Music::Status::Stopped)
            {
                seconds += spareClock.restart().asSeconds();
                if (!isMusicPlayed && seconds >= game->inputOffset / 1000)
                    game->music.play(), spareClock.reset(), isMusicPlayed = true;
            }
            else
                seconds = game->music.getPlayingOffset().asSeconds() + game->inputOffset / 1000;
        }
        else
            seconds = spareClock.getElapsedTime().asSeconds() + game->inputOffset / 1000 + spareClockOffset;
        beat = game->level.seconds2beat(seconds), nowTileIndex = game->level.getFloorByBeat(beat);
    }

    // Update the level
    game->level.update(seconds);

    // Judgement
    if (!waiting)
    {
        if (game->autoplay)
        {
            keyInputCnt = 0;
            for (size_t i = playerTileIndex; i < nowTileIndex; i++)
            {
                if (tiles[i + 1].angle.deg() != 999)
                    keyInputCnt++;
            }
        }
        while (playerTileIndex < tiles.size() - 1 && keyInputCnt-- > 0)
        {
            playerTileIndex++;
            const auto [p, lep, vle] = game->level.getTimingBoundary(playerTileIndex, game->difficulty);
            const double timing = game->level.getTiming(playerTileIndex, seconds),
                         x = std::min(65.0 / 2, std::max(-65.0 / 2, timing / vle * 65.0 / 2.0));
            const AdoCpp::HitMargin hitMargin = game->level.getHitMargin(playerTileIndex, seconds, game->difficulty);
            if (hitMargin == AdoCpp::HitMargin::TooEarly)
            {
                playerTileIndex--;
                if (playerTileIndex == 1)
                    break;
                AdoCpp::Vector2lf pos;
                if (AdoCpp::Level::isFirePlanetStatic(playerTileIndex))
                    pos = game->level.getPlanetsPos(playerTileIndex, seconds).second;
                else
                    pos = game->level.getPlanetsPos(playerTileIndex, seconds).first;
                hitTextSystem.addHitText(seconds, hitMargin, {float(pos.x), float(pos.y)});
            }
            else
            {
                if (playerTileIndex != tiles.size() - 1 && tiles[playerTileIndex + 1].angle.deg() == 999)
                    playerTileIndex++;
                hitTextSystem.addHitText(
                    seconds, hitMargin, {float(tiles[playerTileIndex].pos.c.x), float(tiles[playerTileIndex].pos.c.y)});
            }
            hitErrorMeterSystem.addTick(seconds, hitMargin, x);
        }
        keyInputCnt = 0;
        while (playerTileIndex < tiles.size() - 1 &&
               game->level.getHitMargin(playerTileIndex + 1, seconds, game->difficulty) == AdoCpp::HitMargin::TooLate)
        {
            playerTileIndex++;
            if (tiles[playerTileIndex].angle.deg() != 999)
            {
                hitTextSystem.addHitText(seconds, AdoCpp::HitMargin::TooLate,
                                         {static_cast<float>(tiles[playerTileIndex].pos.c.x),
                                          static_cast<float>(tiles[playerTileIndex].pos.c.y)});
                hitErrorMeterSystem.addTick(seconds, AdoCpp::HitMargin::TooLate, 65.0 / 2);
            }
        }
    }

    // Update planets' positions
    if (!waiting)
    {
        const auto [p1pos, p2pos] = game->level.getPlanetsPos(playerTileIndex, seconds);
        planet1.setPosition({float(p1pos.x), float(p1pos.y)});
        planet2.setPosition({float(p2pos.x), float(p2pos.y)});
    }
    else
    {
        const auto pos = tiles[playerTileIndex].pos.o;
        planet1.setPosition({float(pos.x), float(pos.y)});
    }

    // Update Systems
    game->tileSystem.update();
    hitTextSystem.update(seconds);
    hitErrorMeterSystem.update(seconds);
    hitErrorMeterSystem.setPosition({float(game->windowSize.x) / 2, float(game->windowSize.y) - 100});
    keyViewerSystem.update();
    keyViewerSystem.setPosition({50.f, float(game->windowSize.y) - 500});

    // Update the camera
    game->level.updateCamera(seconds, playerTileIndex);
    const auto [pos, rot, zoom] = game->level.cameraValue();
    game->view.setCenter({float(pos.x), float(pos.y)});
    game->view.setRotation(sf::degrees(float(rot)));
    const auto w = float(game->windowSize.x), h = float(game->windowSize.y);
    /* aw / (aw + ah) = aw / a(w + h) = w / (w + h)
       ah / (aw + ah) = ah / a(w + h) = h / (w + h) */
    game->zoom = {float(zoom) / 100, float(zoom) / 100};
    game->view.setSize({w / (w + h) * 16 * game->zoom.x, -h / (w + h) * 16 * game->zoom.y});
    // ReSharper restore CppFunctionalStyleCast
}

void StatePlaying::render()
{
    auto& tiles = game->level.getTiles();

    // render the world
    game->window.setView(game->view);

    game->window.draw(game->tileSystem);

    game->window.draw(planet1);
    if (!waiting)
        game->window.draw(planet2);

    game->window.draw(hitTextSystem);

    // render the GUI
    sf::View defaultView = game->window.getDefaultView();
    defaultView.setSize(sf::Vector2f(game->windowSize));
    defaultView.setCenter(sf::Vector2f(game->windowSize) / 2.f);
    game->window.setView(defaultView);
    game->window.draw(hitErrorMeterSystem);
    game->window.draw(keyViewerSystem);

    static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()));
    if (ImGui::Begin("LeftText", nullptr, flags))
    {
        ImGui::Text("FPS: %.0f avg, %.0f min, %.0f max", game->avgFps, game->minFps, game->maxFps);
        static double progress, bpm, kps;
        progress = 100 * static_cast<double>(playerTileIndex) / static_cast<double>(tiles.size() - 1);
        bpm = game->level.getBpmByBeat(beat);
        kps = bpm / 60 / (game->level.getAngle(playerTileIndex + (playerTileIndex + 1 == tiles.size() ? 0 : 1)) / 180);
        ImGui::Text("Progress: %.2f%%", progress);
        ImGui::Text("BPM: %.2f", bpm);
        ImGui::Text("KPS: %.2f", kps);
        ImGui::Text("Floor: %llu", nowTileIndex);
    }
    ImGui::End();
}
