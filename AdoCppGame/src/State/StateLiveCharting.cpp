#include "StateLiveCharting.h"

#include "implot.h"

StateLiveCharting StateLiveCharting::m_stateLiveCharting;

void StateLiveCharting::init(Game* _game)
{
    game = _game;

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
void StateLiveCharting::cleanup() {}
void StateLiveCharting::pause() {}
void StateLiveCharting::resume() {}
void StateLiveCharting::handleEvent(const sf::Event event)
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
void StateLiveCharting::update()
{
    const auto w = static_cast<float>(game->windowSize.x), h = static_cast<float>(game->windowSize.y);
    game->view.setSize({w / (w + h) * 16 * game->zoom.x, -h / (w + h) * 16 * game->zoom.y});
    game->tileSystem.setActiveTileIndex(game->activeTileIndex);
    game->tileSystem.update();
}
void StateLiveCharting::render()
{
    // render the world
    game->window.setView(game->view);
    game->window.draw(game->tileSystem);

    // render the GUI
    sf::View defaultView = game->window.getDefaultView();
    defaultView.setSize(sf::Vector2f(game->windowSize));
    defaultView.setCenter(sf::Vector2f(game->windowSize) / 2.f);
    game->window.setView(defaultView);

    if (samples)
    {
        const int rate = soundBuffer->getSampleRate();
        // TODO
        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(game->windowSize.x, -1));
        if (ImGui::Begin("AudioWindow", nullptr, flags)) // FIXME
        {
            if (ImPlot::BeginPlot("Audio"))
            {
                // Thanks to https://github.com/epezent/implot/issues/323
                const double audioLengthInSeconds =
                    static_cast<double>(soundBuffer->getSampleCount() / soundBuffer->getChannelCount()) /
                    soundBuffer->getSampleRate();

                ImPlot::SetupAxes("Time [s]", "Amplitude");
                ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1, ImPlotCond_Always);
                ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, -audioLengthInSeconds / 2, audioLengthInSeconds * 3 / 2);

                const size_t widthPx = ImPlot::GetPlotSize().x;
                double t1 = ImPlot::GetPlotLimits().X.Min;
                double t2 = ImPlot::GetPlotLimits().X.Max;
                static int lastWp;
                static double lastT1, lastT2;
                static std::vector<double> byLow, byHigh, bTime;

                if (widthPx != lastWp || t1 != lastT1 || t2 != lastT2)
                {
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
                ImPlot::PlotLine("##L", bTime.data(), byLow.data(), widthPx);
                ImPlot::PlotLine("##L", bTime.data(), byHigh.data(), widthPx);
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
                    ImPlot::PlotInfLines("##Vertical", &tile.seconds, 1);
                }

                // clang-format off
                const     double dummyX[] = {0, audioLengthInSeconds};
                constexpr double dummyY[] = {1,                   -1};
                // clang-format on
                ImPlot::SetNextLineStyle(ImVec4(0, 0, 0, 0));
                ImPlot::PlotLine("##DummyPointsForFitting", dummyX, dummyY, 4);
                ImPlot::PopStyleVar();
                ImPlot::EndPlot();
            }
        }
        ImGui::End();
    }
}
