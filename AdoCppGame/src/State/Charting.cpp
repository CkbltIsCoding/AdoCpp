#include "Charting.h"
#include "Playing.h"

#include <AudioProcessing.h>
#include <ImGuiFileDialog.h>
#include <exprtk.hpp>
#include <future>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>

static std::map<std::string, char*> buffers;
void ImGuiInputFilename(const char* text, const char* id, const char* hint, std::string* pathPtr)
{
    ImGui::Text(text);
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint(id, hint, pathPtr, ImGuiInputTextFlags_ElideLeft);
}

void ImGuiInputStdString(const char* text, const char* id, std::string* strPtr)
{
    ImGui::Text(text);
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText(id, strPtr);
}

void ImGuiInputDouble(const char* text, const char* id, double* doublePtr)
{
    if (!buffers.contains(id))
        buffers[id] = new char[1145]{}, sprintf_s(buffers[id], 1145, "%g", *doublePtr);

    ImGui::Text(text);
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText(id, buffers[id], 1145, ImGuiInputTextFlags_CharsDecimal);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        exprtk::expression<double> expression;
        exprtk::parser<double> parser;
        parser.compile((std::string)buffers[id], expression);
        *doublePtr = expression.value();
        sprintf_s(buffers[id], 1145, "%g", *doublePtr);
    }
    if (!ImGui::IsItemActive())
        sprintf_s(buffers[id], 1145, "%g", *doublePtr);
}


void ImGuiInputFloat(const char* text, const char* id, double* floatPtr)
{
    if (!buffers.contains(id))
        buffers[id] = new char[1145]{}, sprintf_s(buffers[id], 1145, "%g", *floatPtr);

    ImGui::Text(text);
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText(id, buffers[id], 1145, ImGuiInputTextFlags_CharsDecimal);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        exprtk::expression<float> expression;
        exprtk::parser<float> parser;
        parser.compile((std::string)buffers[id], expression);
        *floatPtr = expression.value();
        sprintf_s(buffers[id], 1145, "%g", *floatPtr);
    }
    if (!ImGui::IsItemActive())
        sprintf_s(buffers[id], 1145, "%g", *floatPtr);
}


StateCharting StateCharting::m_stateCharting;

void StateCharting::init(Game* l_game)
{
    game = l_game;
    newLevel();
}

void StateCharting::cleanup() {}

void StateCharting::pause() {}

void StateCharting::resume()
{
    game->activeTileIndex = std::nullopt;
    game->level.update();
    game->tileSystem.update();
    game->view.setRotation(sf::degrees(0));
}

void StateCharting::handleEvent(sf::Event event)
{
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::F12)
                game->autoplay = !game->autoplay;
            else if (keyPressed->code == sf::Keyboard::Key::Space)
                game->level.initCamera(), game->pushState(StatePlaying::instance());
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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))
            {
                if (d.x > 0 || d.y > 0)
                    game->zoom /= 1.5f;
                else
                    game->zoom *= 1.5f;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift))
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
                    auto mouseCoords = game->window.mapPixelToCoords(mbp->position);
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

void StateCharting::update()
{
    const auto w = static_cast<float>(game->windowSize.x), h = static_cast<float>(game->windowSize.y);
    game->view.setSize({w / (w + h) * 16 * game->zoom.x, -h / (w + h) * 16 * game->zoom.y});
    game->tileSystem.setActiveTileIndex(game->activeTileIndex);
    game->tileSystem.update();
}

void StateCharting::render()
{
    auto& tiles = game->level.getTiles();

    // render the world
    game->window.setView(game->view);
    game->window.draw(game->tileSystem);

    // render the GUI
    sf::View defaultView = game->window.getDefaultView();
    defaultView.setSize(sf::Vector2f(game->windowSize));
    defaultView.setCenter(sf::Vector2f(game->windowSize) / 2.f);
    game->window.setView(defaultView);

    ImGui::ShowDemoWindow();

    static constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    static float barWidth;
    barWidth = ImGui::GetFontSize() * 15;
    ImGui::SetNextWindowSize(ImVec2(barWidth, 0));
    ImGui::SetNextWindowPos(ImVec2(game->windowSize.x / 2.f - barWidth / 2.f, 0));
    if (ImGui::Begin("FilenameBar", nullptr, flags))
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        static std::future<std::filesystem::path> future;
        static float progress;
        std::string filename = game->levelPath.filename().string();
        if (filename.empty())
            filename = "Untitled";
        if (ImGui::TreeNodeEx("FilenameBarTreeNode", ImGuiTreeNodeFlags_SpanAvailWidth, (filename).c_str()))
        {
            if (ImGui::Button("Open ...", ImVec2(-1, 0)))
            {
                IGFD::FileDialogConfig config;
                config.path = ".";
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose an ADOFAI file", ".adofai", config);
            }
            if (addedHitsound)
                ImGui::BeginDisabled();
            if (ImGui::Button("Add hitsound", ImVec2(-1, 0)))
            {
                if (!addedHitsound)
                {
                    future = std::async(std::launch::async, addHitsound, game->musicPath, tiles, &progress, false);
                    ImGui::OpenPopup("Adding hitsound...");
                }
            }
            if (addedHitsound)
                ImGui::EndDisabled();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Adding hitsound...", nullptr))
            {
                if (game->musicPath.empty())
                {
                    ImGui::Text("AdoCpp cannot add hitsound\n because the music file name is empty.");
                    if (ImGui::Button("Close"))
                        ImGui::CloseCurrentPopup();
                }
                else
                {
                    if (progress == -1)
                        ImGui::Text("Loading...");
                    else if (progress == 2)
                        ImGui::Text("Saving...");
                    else
                        ImGui::Text("Adding hitsound...");
                    ImGui::ProgressBar(std::max(0.f, std::min(1.f, progress)), ImVec2(-1, 0));
                    if (future.valid() && future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            game->musicPath = future.get();

                            addedHitsound = true;
                            if (!game->music.openFromFile(game->musicPath))
                            {
                            }
                            ImGui::CloseCurrentPopup();
                        }
                        catch (const std::exception& ex)
                        {
                            ImGui::Text("An error occurred. %s", ex.what());
                            if (ImGui::Button("Close"))
                                ImGui::CloseCurrentPopup();
                        }
                    }
                }
                ImGui::EndPopup();
            }
            ImGui::TreePop();
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                game->levelPath = ImGuiFileDialog::Instance()->GetFilePathName();
                try
                {
                    game->level.fromFile(game->levelPath);
                }
                catch (const AdoCpp::LevelJsonException&)
                {
                    game->levelPath.clear();
                    game->level.defaultLevel();
                    ImGui::OpenPopup("Error!##AdoCpp::LevelJsonHasParseErrorException");
                }
                newLevel();
            }
            ImGuiFileDialog::Instance()->Close();
        }
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Error!##AdoCpp::LevelJsonHasParseErrorException", nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Error when parsing json.");
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }
    ImGui::End();

    static float leftWidth, leftHeight;
    leftWidth = ImGui::GetFontSize() * 15, leftHeight = ImGui::GetFontSize() * 30;
    ImGui::SetNextWindowSize(ImVec2(leftWidth, leftHeight));
    ImGui::SetNextWindowPos(ImVec2(0, game->windowSize.y / 2.f - leftHeight / 2.f));
    if (ImGui::Begin("LeftSettings", nullptr, flags))
    {
        auto& settings = game->level.getSettings();
        static float leftSettingsTabContentWidth;
        leftSettingsTabContentWidth = leftWidth / 5 * 4;
        static size_t selectedTab = 0;
        static const std::array<std::string, 7> titles = {
            "Song Settings",   "Level Settings",         "Track Settings", "Background Settings",
            "Camera Settings", "Miscellaneous Settings", "Decorations"};
        if (ImGui::BeginChild("LeftSettings/TabContent", ImVec2(leftSettingsTabContentWidth, 0)))
        {
            ImGui::SetCursorPosX(leftSettingsTabContentWidth / 2 -
                                 ImGui::CalcTextSize(titles[selectedTab].c_str()).x / 2);
            ImGui::Text(titles[selectedTab].c_str());
#define AD_PREFIX "LeftSettings/TabContent/SongSettings/"
            switch (selectedTab)
            {
            case 0: // Song
                {
                    ImGuiInputFilename("Song Filename:", AD_PREFIX "SongFilename", "No files selected",
                                       &settings.songFilename);
                    ImGuiInputDouble("BPM:", AD_PREFIX "BPM", &settings.bpm);
                    ImGuiInputDouble("Volume:", AD_PREFIX "Volume", &settings.volume);
                    ImGuiInputDouble("Offset:", AD_PREFIX "Offset", &settings.offset);
                    ImGuiInputDouble("Pitch:", AD_PREFIX "Pitch", &settings.pitch);
                    ImGui::Text("Hitsound:"); // ImGui::SetNextItemWidth(-1);
                    ImGuiInputDouble("Hitsound Volume:", AD_PREFIX "HitSoundVolume", &settings.hitsoundVolume);
                    ImGuiInputDouble("Countdown Ticks:", AD_PREFIX "CountdownTicks", &settings.countdownTicks);
                    break;
                }
            case 1: // Level
                {
#undef AD_PREFIX
#define AD_PREFIX "LeftSettings/TabContent/LevelSettings/"
                    ImGuiInputStdString("Artist:", AD_PREFIX "Artist", &settings.artist);
                    ImGuiInputStdString("Song:", AD_PREFIX "Song", &settings.song);
                    ImGuiInputStdString("Author:", AD_PREFIX "Author", &settings.author);
                    break;
                }
            case 2: // Track
                {
#undef AD_PREFIX
#define AD_PREFIX "LeftSettings/TabContent/TrackSettings/"
                    break;
                }
            default:
                break;
            }
        }
#undef AD_PREFIX
        ImGui::EndChild();
        ImGui::SameLine();
        if (ImGui::BeginChild("LeftSettings/TabBtns"))
        {
            if (ImGui::BeginTable("LeftSettings/TabBtns/Table", 1))
            {
                for (size_t i = 0; i < 7; i++)
                {
                    ImGui::TableNextRow(), ImGui::TableNextColumn();
                    ImGui::PushID(("LeftSettings/TabBtns/Table/TabBtn" + std::to_string(i)).c_str());
                    if (ImGui::Button(titles[i].c_str(), ImVec2(-1, 0)))
                        selectedTab = i;
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();

    const auto tile = game->activeTileIndex.has_value() ? &tiles[*game->activeTileIndex] : nullptr;
    static bool rightSettingsOpen;
    rightSettingsOpen = game->activeTileIndex.has_value();
    static float rightWidth, rightHeight;
    rightWidth = ImGui::GetFontSize() * 15, rightHeight = ImGui::GetFontSize() * 30;
    if (rightSettingsOpen)
    {
        ImGui::SetNextWindowSize(ImVec2(rightWidth, rightHeight));
        ImGui::SetNextWindowPos(
            ImVec2(game->windowSize.x - rightWidth, game->windowSize.y / 2.f - rightHeight / 2.f));
        if (ImGui::Begin("RightSettings", nullptr, flags))
        {
            static float rightSettingsTabBtnsWidth, rightSettingsTabContentWidth;
            rightSettingsTabBtnsWidth = rightWidth / 5,
            rightSettingsTabContentWidth = rightWidth - rightSettingsTabBtnsWidth;
            static std::optional<size_t> tileIndex;
            static size_t selectedTab = 0;
            if (game->activeTileIndex != tileIndex)
                tileIndex = game->activeTileIndex, selectedTab = 0;
            if (ImGui::BeginChild("RightSettings/TabBtns", ImVec2(rightSettingsTabBtnsWidth, 0)))
            {
                if (ImGui::BeginTable("RightSettings/TabBtns/Table", 1))
                {
                    if (tile)
                    {
                        for (size_t i = 0; i < tile->events.size(); i++)
                        {
                            ImGui::TableNextRow(), ImGui::TableNextColumn();
                            ImGui::PushID(("RightSettings/TabBtns/Table/TabBtn" + std::to_string(i)).c_str());
                            if (ImGui::Button(tile->events[i]->name(), ImVec2(-1, 0)))
                                selectedTab = i;
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::EndChild();
            ImGui::SameLine();
            if (ImGui::BeginChild("RightSettings/TabContent"))
            {
                if (tile && !tile->events.empty())
                {
                    std::string title = tile->events[selectedTab]->name();
                    ImGui::SetCursorPosX(rightSettingsTabContentWidth / 2 - ImGui::CalcTextSize(title.c_str()).x / 2);
                    ImGui::Text(title.c_str());
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    if (ImGui::Begin("ControlPad"))
    {
        if (ImGui::TreeNode("DifficultySelect"))
        {
            using AdoCpp::Difficulty;

            if (ImGui::Selectable("Lenient", game->difficulty == Difficulty::Lenient))
                game->difficulty = Difficulty::Lenient;
            if (ImGui::Selectable("Normal", game->difficulty == Difficulty::Normal))
                game->difficulty = Difficulty::Normal;
            if (ImGui::Selectable("Strict", game->difficulty == Difficulty::Strict))
                game->difficulty = Difficulty::Strict;

            ImGui::TreePop();
        }
        ImGui::SliderFloat("InputOffsetSlider", &game->inputOffset, -250, 250, "%.0f");
    }
    ImGui::End();
}

void StateCharting::newLevel()
{
    addedHitsound = false;
    game->activeTileIndex = std::nullopt;
    game->level.parse();
    game->level.update();
    game->tileSystem.parse();
    game->tileSystem.update();
    game->musicPath = game->levelPath.parent_path().append(game->level.getSettings().songFilename);
    if (!game->musicPath.empty())
    {
        if (!game->music.openFromFile(game->musicPath))
        {
            std::cerr << "Warning: failed to load music from file \"" << game->musicPath
                      << "\". Maybe the file does not exist or it is not a music file.";
        }
    }
    game->view.setCenter({0.f, 0.f});
    game->view.setRotation(sf::degrees(0));
    game->zoom = {1.f, 1.f};
}
