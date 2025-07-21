#include "LiveCharting.h"

#include <map>

#include "implot.h"

LiveCharting LiveCharting::m_stateLiveCharting;

void LiveCharting::init(Game* _game)
{
    game = _game;

    render_needToUpdateOscillogram = true;

    planet1.setFillColor(sf::Color::Red);
    planet2.setFillColor(sf::Color::Blue);
    planet1.setRadius(0.25);
    planet2.setRadius(0.25);
    planet1.setOrigin({planet1.getRadius(), planet1.getRadius()});
    planet2.setOrigin({planet2.getRadius(), planet2.getRadius()});
    if (!game->origMusicPath.empty())
    {
        try
        {
            soundBuffer = sf::SoundBuffer(game->origMusicPath);
            music = sf::Sound(*soundBuffer);
            const int16_t* origSamples = soundBuffer->getSamples();
            const size_t sampleCount = soundBuffer->getSampleCount();
            samples = std::vector<double>(sampleCount);
            int16_t maxSample = 1;
            for (size_t i = 0; i < sampleCount; i++)
                maxSample = std::max(maxSample, static_cast<int16_t>(std::abs(origSamples[i])));
            for (size_t i = 0; i < sampleCount; i++)
                (*samples)[i] = static_cast<double>(origSamples[i]) / maxSample;
        }
        catch (std::exception& ex)
        {
            soundBuffer = std::nullopt;
        }
    }
}
void LiveCharting::cleanup()
{
    game->level.parse(true);
    game->level.update();
    game->tileSystem.parse();
    game->tileSystem.update();
}
void LiveCharting::pause() {}
void LiveCharting::resume() {}
void LiveCharting::handleEvent(const sf::Event event)
{
    using enum sf::Keyboard::Key;
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == F12)
                game->autoplay = !game->autoplay;
            else if (keyPressed->code == Escape)
                game->popState();
            const std::map<sf::Keyboard::Key, double> keyMap = {{D, 0},   {E, 45},  {W, 90},  {Q, 135},
                                                                {A, 180}, {Z, 225}, {X, 270}, {C, 315}};
            const std::map<sf::Keyboard::Key, double> shiftKeyMap = {{J, 30},  {Y, 60},  {T, 120}, {H, 150},
                                                                     {N, 210}, {V, 240}, {B, 300}, {M, 330}};
            const std::map<sf::Keyboard::Key, double> shiftGraveKeyMap = {{J, 15},  {Y, 75},  {T, 105}, {H, 165},
                                                                          {N, 195}, {V, 255}, {B, 285}, {M, 345}};
            for (const auto& [key, value] : !sf::Keyboard::isKeyPressed(LShift) ? keyMap
                     : !sf::Keyboard::isKeyPressed(Grave)                       ? shiftKeyMap
                                                                                : shiftGraveKeyMap)
                if (keyPressed->code == key)
                {
                    if (game->activeTileIndex)
                    {
                        game->level.insertTile(*game->activeTileIndex + 1, value);
                        (*game->activeTileIndex)++;
                    }
                    else
                    {
                        game->level.pushBackTile(value);
                        game->activeTileIndex = game->level.getTiles().size() - 1;
                    }
                    parseUpdateLevel();
                }

            if (game->activeTileIndex && (keyPressed->code == Backspace || keyPressed->code == Delete))
            {
                // if (sf::Keyboard::isKeyPressed(LControl))
                // {
                //     if (keyPressed->code == Backspace)
                //         game->level.eraseTile(0, *game->activeTileIndex), *game->activeTileIndex = 0;
                //     else
                //         game->level.eraseTile(*game->activeTileIndex + 1,
                //                               game->level.getTiles().size());
                // }
                // else
                // {
                //     game->level.eraseTile(*game->activeTileIndex, *game->activeTileIndex + 1);
                //     (*game->activeTileIndex)--;
                // }
                // parseUpdateLevel();
            }
        }
    }
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>())
        {
            static sf::Vector2f d;
            if (mws->wheel == sf::Mouse::Wheel::Vertical)
                d = {0, mws->delta * game->zoom.y};
            else
                d = {-mws->delta * game->zoom.y, 0};
            if (sf::Keyboard::isKeyPressed(LControl) || sf::Keyboard::isKeyPressed(RControl))
            {
                if (d.x > 0 || d.y > 0)
                    game->zoom /= 1.5f;
                else
                    game->zoom *= 1.5f;
            }
            else if (sf::Keyboard::isKeyPressed(LShift) || sf::Keyboard::isKeyPressed(RShift))
            {
                game->view.move({-d.y, -d.x});
            }
            else
            {
                game->view.move(d);
            }
        }
        if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mbp->button == sf::Mouse::Button::Left)
            {
                for (size_t i = 0; i < game->level.getTiles().size(); i++)
                {
                    game->window.setView(game->view);
                    const auto mouseCoords = game->window.mapPixelToCoords(mbp->position);
                    if (game->tileSystem[i].isPointInside(mouseCoords))
                    {
                        game->activeTileIndex = i;
                        return;
                    }
                }
                game->activeTileIndex = std::nullopt;
            }
        }
    }
}
void LiveCharting::update()
{
    const auto w = static_cast<float>(game->windowSize.x), h = static_cast<float>(game->windowSize.y);
    game->view.setSize({w / (w + h) * 16 * game->zoom.x, -h / (w + h) * 16 * game->zoom.y});
    game->tileSystem.setActiveTileIndex(game->activeTileIndex);
    game->tileSystem.update();
}
void LiveCharting::render()
{
    // render the world
    game->window.setView(game->view);
    game->window.draw(game->tileSystem);

    // render the GUI
    sf::View defaultView = game->window.getDefaultView();
    defaultView.setSize(sf::Vector2f(game->windowSize));
    defaultView.setCenter(sf::Vector2f(game->windowSize) / 2.f);
    game->window.setView(defaultView);

    renderAudioWindow();
    renderEventBar();
    renderEventSettings();
}
void LiveCharting::renderAudioWindow()
{
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(game->windowSize.x, -1));
    if (ImGui::Begin("AudioWindow", nullptr, flags)) // FIXME
    {
        if (ImPlot::BeginPlot("Audio"))
        {
            // Thanks to https://github.com/epezent/implot/issues/323
            ImPlot::SetupAxes("Time [s]", "Amplitude");
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1, ImPlotCond_Always);

            if (samples)
            {
                const double audioLengthInSeconds =
                    static_cast<double>(soundBuffer->getSampleCount() / soundBuffer->getChannelCount()) /
                    soundBuffer->getSampleRate();
                ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, -audioLengthInSeconds / 2, audioLengthInSeconds * 3 / 2);
                const size_t widthPx = ImPlot::GetPlotSize().x;
                double t1 = ImPlot::GetPlotLimits().X.Min;
                double t2 = ImPlot::GetPlotLimits().X.Max;
                static int lastWp;
                static double lastT1, lastT2;
                static std::vector<double> byLow, byHigh, bTime;

                if (widthPx != lastWp || t1 != lastT1 || t2 != lastT2 && render_needToUpdateOscillogram)
                {
                    render_needToUpdateOscillogram = false;
                    lastWp = widthPx, lastT1 = t1, lastT2 = t2;
                    t1 = std::max(t1, 0.0), t2 = std::min(t2, audioLengthInSeconds);
                    byLow.resize(widthPx), byHigh.resize(widthPx), bTime.resize(widthPx);
                    // clang-format off
                    const double frames      = floor((t2 - t1) * soundBuffer->getSampleRate());
                    const double framesPerPx = frames / widthPx;
                    const int    binLength   = std::max(1.0, floor(framesPerPx + 0.5));
                    const int    framesTotal = soundBuffer->getSampleCount() / soundBuffer->getChannelCount();
                    for (int i = 0; i < widthPx; ++i)
                    {
                        const int binBegin_ = floor(t1 * soundBuffer->getSampleRate() + i * framesPerPx);
                        const int binBegin  = std::max(0, std::min(framesTotal - 1, binBegin_));
                        const int binEnd    = std::max(0, std::min(framesTotal - 1, binBegin_ + binLength - 1));
                        bTime[i]  =  t1 + i * (t2 - t1) / static_cast<double>(widthPx - 1);
                        byLow[i]  =  std::numeric_limits<double>::infinity();
                        byHigh[i] = -std::numeric_limits<double>::infinity();
                        for (int j = binBegin; j <= binEnd; ++j)
                        {
                            const int j1 = j * soundBuffer->getChannelCount();
                            double value = samples->at(j1);
                            byLow[i]     = std::min(byLow[i], value);
                            byHigh[i]    = std::max(byHigh[i], value);
                        }
                    }
                    // clang-format on
                }
                ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
                ImPlot::PlotShaded("##L", bTime.data(), byLow.data(), byHigh.data(), widthPx);
                ImPlot::PopStyleVar();
                ImPlot::PlotLine("##L", bTime.data(), byLow.data(), widthPx);
                ImPlot::PlotLine("##L", bTime.data(), byHigh.data(), widthPx);

                // clang-format off
                const     double dummyX[] = {0, audioLengthInSeconds};
                constexpr double dummyY[] = {1,                   -1};
                // clang-format on
                ImPlot::SetNextLineStyle(ImVec4(0, 0, 0, 0));
                ImPlot::PlotLine("##DummyPointsForFitting", dummyX, dummyY, 4);
            }

            const auto& tiles = game->level.getTiles();
            for (size_t i = 0; i < tiles.size(); ++i)
            {
                const auto& tile = tiles[i];
                if (game->activeTileIndex && *game->activeTileIndex == i)
                {
                    ImPlot::TagX(tile.seconds, ImVec4(0, 1, 0, 1));
                    ImPlot::SetNextLineStyle(ImVec4(0, 1, 0, 1));
                }
                else
                    ImPlot::SetNextLineStyle(ImVec4(1, 1, 0, 1));
                ImPlot::PlotInfLines("##TileSecond", &tile.seconds, 1);
            }

            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}
void LiveCharting::renderEventBar()
{
    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    const float barWidth = ImGui::GetFontSize() * 30, barHeight = ImGui::GetFontSize() * 5;
    ImGui::SetNextWindowPos(ImVec2(game->windowSize.x / 2.f - barWidth / 2.f, game->windowSize.y - barHeight));
    ImGui::SetNextWindowSize(ImVec2(barWidth, barHeight));
    if (game->activeTileIndex)
    {
        if (ImGui::Begin("EventBar", nullptr, flags)) // FIXME
        {
            if (ImGui::BeginTabBar("EventTabBar"))
            {
                using namespace AdoCpp::Event;
                if (ImGui::BeginTabItem("GamePlay"))
                {
                    using namespace GamePlay;
                    if (ImGui::Button("Set Speed"))
                    {
                        const auto e = new SetSpeed();
                        e->floor = *game->activeTileIndex;
                        game->level.addEvent(e, 0), parseUpdateLevel();
                    }
                    if (ImGui::Button("Twirl"))
                    {
                        const auto e = new Twirl();
                        e->floor = *game->activeTileIndex;
                        game->level.addEvent(e, 0), parseUpdateLevel();
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Track"))
                {
                    using namespace Track;
                    if (ImGui::Button("Position Track"))
                    {
                        const auto e = new PositionTrack();
                        e->floor = *game->activeTileIndex;
                        game->level.addEvent(e, 0), parseUpdateLevel();
                    }
                    if (ImGui::Button("Set Track Color"))
                    {
                        const auto e = new ColorTrack();
                        e->floor = *game->activeTileIndex;
                        game->level.addEvent(e, 0), parseUpdateLevel();
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }
}
void LiveCharting::renderEventSettings()
{
    if (game->activeTileIndex)
    {
        auto& tile = game->level.getTiles()[*game->activeTileIndex];
        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
        const float width = ImGui::GetFontSize() * 15, height = ImGui::GetFontSize() * 30;
        ImGui::SetNextWindowPos(ImVec2(game->windowSize.x - width, game->windowSize.y - height));
        ImGui::SetNextWindowSize(ImVec2(width, height));
        if (ImGui::Begin("Event", nullptr, flags)) // FIXME
        {
            static float rightSettingsTabBtnsWidth, rightSettingsTabContentWidth;
            rightSettingsTabBtnsWidth = width / 5, rightSettingsTabContentWidth = width - rightSettingsTabBtnsWidth;
            static std::optional<size_t> tileIndex;
            static size_t selectedTab = 0;
            if (game->activeTileIndex != tileIndex)
                tileIndex = game->activeTileIndex, selectedTab = 0;
            if (ImGui::BeginChild("EventSettings/TabBtns", ImVec2(rightSettingsTabBtnsWidth, 0)))
            {
                if (ImGui::BeginTable("EventSettings/TabBtns/Table", 1))
                {
                    for (size_t i = 0; i < tile.events.size(); i++)
                    {
                        ImGui::TableNextRow(), ImGui::TableNextColumn();
                        ImGui::PushID(("EventSettings/TabBtns/Table/TabBtn" + std::to_string(i)).c_str());
                        if (ImGui::Button(tile.events[i]->name(), ImVec2(-1, 0)))
                            selectedTab = i;
                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::EndChild();
            ImGui::SameLine();
            if (ImGui::BeginChild("EventSettings/TabContent"))
            {
                if (!tile.events.empty())
                {
                    const char* title = tile.events[selectedTab]->name();
                    ImGui::SetCursorPosX(rightSettingsTabContentWidth / 2 - ImGui::CalcTextSize(title).x / 2);
                    ImGui::Text(title);
                    AdoCpp::Event::Event* event = tile.events[selectedTab];
                    using namespace AdoCpp::Event::GamePlay;
                    if (const auto twirl = dynamic_cast<AdoCpp::Event::GamePlay::Twirl*>(event))
                    {
                    }
                    if (const auto setSpeed = dynamic_cast<AdoCpp::Event::GamePlay::SetSpeed*>(event))
                    {
                        ImGui::Text("SpeedType");
                        ImGui::SameLine();
                        ImGui::RadioButton("BPM", reinterpret_cast<int*>(&setSpeed->speedType),
                                           static_cast<int>(SetSpeed::SpeedType::Bpm));
                        ImGui::SameLine();
                        ImGui::RadioButton("Multiplier", reinterpret_cast<int*>(&setSpeed->speedType),
                                           static_cast<int>(SetSpeed::SpeedType::Multiplier));
                        if (setSpeed->speedType == SetSpeed::SpeedType::Multiplier)
                            ImGui::BeginDisabled();
                        ImGui::InputDouble("Beats Per Minute##SetSpeed", &setSpeed->beatsPerMinute);
                        if (setSpeed->speedType == SetSpeed::SpeedType::Multiplier)
                            ImGui::EndDisabled();
                        if (setSpeed->speedType == SetSpeed::SpeedType::Bpm)
                            ImGui::BeginDisabled();
                        ImGui::InputDouble("BPM Multiplier##SetSpeed", &setSpeed->bpmMultiplier);
                        if (setSpeed->speedType == SetSpeed::SpeedType::Bpm)
                            ImGui::EndDisabled();
                    }
                }
            }
            ImGui::EndChild();
            if (ImGui::Button("Delete"))
            {
                game->level.removeEvent(*game->activeTileIndex, selectedTab);
                parseUpdateLevel();
            }
        }
        ImGui::End();
    }
}
void LiveCharting::parseUpdateLevel()
{
    game->level.parse(true);
    game->level.update();
    game->tileSystem.parse();
    game->tileSystem.update();
}
