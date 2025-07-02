#include "Charting.h"
#include "Playing.h"

#include <AudioProcessing.h>
#include <IconsFontAwesome6.h>
#include <ImGuiFileDialog.h>
#include <exprtk.hpp>
#include <future>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>

StateCharting StateCharting::m_stateCharting;

void StateCharting::init(Game* game)
{
    m_game = game;
    newLevel();
}

void StateCharting::cleanup() {}

void StateCharting::pause() {}

void StateCharting::resume()
{
    m_game->activeTileIndex = std::nullopt;
    m_game->level.update();
    m_game->tileSystem.update();
    m_game->view.setRotation(sf::degrees(0));
}

void StateCharting::handleEvent(sf::Event event)
{
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::F12)
                m_game->autoplay = !m_game->autoplay;
            else if (keyPressed->code == sf::Keyboard::Key::Space)
                m_game->level.initCamera(), m_game->pushState(StatePlaying::instance());
        }
    }
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>())
        {
            static sf::Vector2f d;
            if (mws->wheel == sf::Mouse::Wheel::Vertical)
                d = {0, mws->delta * m_game->zoom.y};
            else
                d = {-mws->delta * m_game->zoom.y, 0};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))
            {
                if (d.x > 0 || d.y > 0)
                    m_game->zoom /= 1.5f;
                else
                    m_game->zoom *= 1.5f;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift))
            {
                m_game->view.move({-d.y, -d.x});
            }
            else
            {
                m_game->view.move(d);
            }
        }
        if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mbp->button == sf::Mouse::Button::Left)
            {
                for (size_t i = 0; i < m_game->level.tiles.size(); i++)
                {
                    m_game->window.setView(m_game->view);
                    auto mouseCoords = m_game->window.mapPixelToCoords(mbp->position);
                    if (m_game->tileSystem[i].isPointInside(mouseCoords))
                    {
                        m_game->activeTileIndex = i;
                        return;
                    }
                }
                m_game->activeTileIndex = std::nullopt;
            }
        }
    }
}

void StateCharting::update()
{
    const auto w = static_cast<float>(m_game->windowSize.x), h = static_cast<float>(m_game->windowSize.y);
    m_game->view.setSize({w / (w + h) * 16 * m_game->zoom.x, -h / (w + h) * 16 * m_game->zoom.y});
    m_game->tileSystem.setActiveTileIndex(m_game->activeTileIndex);
    m_game->tileSystem.update();
}

void StateCharting::render()
{
    // render the world
    m_game->window.setView(m_game->view);
    m_game->window.draw(m_game->tileSystem);

    // render the GUI
    sf::View defaultView = m_game->window.getDefaultView();
    defaultView.setSize(sf::Vector2f(m_game->windowSize));
    defaultView.setCenter(sf::Vector2f(m_game->windowSize) / 2.f);
    m_game->window.setView(defaultView);

    ImGui::ShowDemoWindow();

    static constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    static float barWidth;
    barWidth = ImGui::GetFontSize() * 15;
    ImGui::SetNextWindowSize(ImVec2(barWidth, 0));
    ImGui::SetNextWindowPos(ImVec2(m_game->windowSize.x / 2.f - barWidth / 2.f, 0));
    if (ImGui::Begin("FilenameBar", nullptr, flags))
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        static std::future<std::filesystem::path> future;
        static float progress;
        std::string filename = m_game->levelPath.filename().string();
        if (filename.empty())
            filename = "Untitled";
        if (ImGui::TreeNodeEx("FilenameBarTreeNode", ImGuiTreeNodeFlags_SpanAvailWidth, (filename).c_str()))
        {
            if (ImGui::Button("Open ...", ImVec2(-1, 0)))
            {
                IGFD::FileDialogConfig config;
                config.path = ".";
                config.flags = ImGuiFileDialogFlags_Modal;
                // ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(0.8f, 0.8f, 0.8f, 1),
                //                                           " " ICON_FA_FOLDER " ");
                // ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".adofai", ImVec4(1, 1, 1, 1),
                //                                           " " ICON_FA_FILE " ");
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose an ADOFAI file", ".adofai", config);
            }
            if (addedHitsound)
                ImGui::BeginDisabled();
            if (ImGui::Button("Add hitsound", ImVec2(-1, 0)))
            {
                if (!addedHitsound)
                {
                    std::vector<double> vector(m_game->level.tiles.size() - 1);
                    for (size_t i = 1 /* tile[0].beat: -INF */; i < m_game->level.tiles.size(); i++)
                    {
                        vector[i - 1] = m_game->level.tiles[i].seconds;
                    }
                    future = std::async(std::launch::async, addHitsound, m_game->musicPath, vector, &progress, false);
                    ImGui::OpenPopup("Adding hitsound...");
                }
            }
            if (addedHitsound)
                ImGui::EndDisabled();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Adding hitsound...", nullptr))
            {
                if (m_game->musicPath.empty())
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
                            m_game->musicPath = future.get();

                            addedHitsound = true;
                            if (!m_game->music.openFromFile(m_game->musicPath))
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
                m_game->levelPath = ImGuiFileDialog::Instance()->GetFilePathName();
                m_game->level.clear();
                try
                {
                    m_game->level.fromFile(m_game->levelPath);
                }
                catch (const AdoCpp::LevelJsonException&)
                {
                    m_game->levelPath.clear();
                    m_game->level.defaultLevel();
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
    ImGui::SetNextWindowPos(ImVec2(0, m_game->windowSize.y / 2.f - leftHeight / 2.f));
    if (ImGui::Begin("LeftSettings", nullptr, flags))
    {
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
            switch (selectedTab)
            {
            case 0: // Song
                {
                    ImGuiInputFilename("Song Filename:", "LeftSettings/TabContent/SongSettings/SongFilename",
                                       "No files selected", &m_game->level.settings.songFilename);
                    ImGuiInputDouble("BPM:", "LeftSettings/TabContent/SongSettings/BPM", &m_game->level.settings.bpm);
                    ImGuiInputDouble("Volume:", "LeftSettings/TabContent/SongSettings/Volume",
                                     &m_game->level.settings.volume);
                    ImGuiInputDouble("Offset:", "LeftSettings/TabContent/SongSettings/Offset",
                                     &m_game->level.settings.offset);
                    ImGuiInputDouble("Pitch:", "LeftSettings/TabContent/SongSettings/Pitch",
                                     &m_game->level.settings.pitch);
                    ImGui::Text("Hitsound:"); // ImGui::SetNextItemWidth(-1);
                    ImGuiInputDouble("Hitsound Volume:", "LeftSettings/TabContent/SongSettings/HitSoundVolume",
                                     &m_game->level.settings.hitsoundVolume);
                    ImGuiInputDouble("Countdown Ticks:", "LeftSettings/TabContent/SongSettings/CountdownTicks",
                                     &m_game->level.settings.countdownTicks);
                    break;
                }
            case 1: // Level
                {
                    break;
                }
            case 2:
                {
                    break;
                }
            default:
                break;
            }
        }
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

    const auto tile = m_game->activeTileIndex.has_value() ? &m_game->level.tiles[*m_game->activeTileIndex] : nullptr;
    static bool rightSettingsOpen;
    rightSettingsOpen = m_game->activeTileIndex.has_value();
    static float rightWidth, rightHeight;
    rightWidth = ImGui::GetFontSize() * 15, rightHeight = ImGui::GetFontSize() * 30;
    if (rightSettingsOpen)
    {
        ImGui::SetNextWindowSize(ImVec2(rightWidth, rightHeight));
        ImGui::SetNextWindowPos(
            ImVec2(m_game->windowSize.x - rightWidth, m_game->windowSize.y / 2.f - rightHeight / 2.f));
        if (ImGui::Begin("RightSettings", nullptr, flags))
        {
            static float rightSettingsTabBtnsWidth, rightSettingsTabContentWidth;
            rightSettingsTabBtnsWidth = rightWidth / 5,
            rightSettingsTabContentWidth = rightWidth - rightSettingsTabBtnsWidth;
            static std::optional<size_t> tileIndex;
            static size_t selectedTab = 0;
            if (m_game->activeTileIndex != tileIndex)
                tileIndex = m_game->activeTileIndex, selectedTab = 0;
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
                            if (ImGui::Button(tile->events[i]->name().c_str(), ImVec2(-1, 0)))
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

            if (ImGui::Selectable("Lenient", m_game->difficulty == Difficulty::Lenient))
                m_game->difficulty = Difficulty::Lenient;
            if (ImGui::Selectable("Normal", m_game->difficulty == Difficulty::Normal))
                m_game->difficulty = Difficulty::Normal;
            if (ImGui::Selectable("Strict", m_game->difficulty == Difficulty::Strict))
                m_game->difficulty = Difficulty::Strict;

            ImGui::TreePop();
        }
        ImGui::SliderFloat("InputOffsetSlider", &m_game->inputOffset, -250, 250, "%.0f");
    }
    ImGui::End();
}

void StateCharting::newLevel()
{
    addedHitsound = false;
    m_game->activeTileIndex = std::nullopt;
    m_game->level.parse();
    m_game->level.update();
    m_game->tileSystem.parse();
    m_game->tileSystem.update();
    m_game->musicPath = m_game->levelPath.parent_path().append(m_game->level.settings.songFilename);
    if (!m_game->musicPath.empty())
    {
        if (!m_game->music.openFromFile(m_game->musicPath))
        {
            std::cerr << "Warning: failed to load music from file \"" << m_game->musicPath
                      << "\". Maybe the file does not exist or it is not a music file.";
        }
    }
    m_game->view.setCenter({float(m_game->level.tiles[0].pos.c.x), float(m_game->level.tiles[0].pos.c.y)});
    m_game->view.setRotation(sf::degrees(0));
    m_game->zoom = {1.f, 1.f};
}

static std::map<std::string, char*> buffers;
void StateCharting::ImGuiInputFilename(const char* text, const char* id, const char* hint, std::string* pathPtr)
{
    if (buffers.find(id) == buffers.end())
        buffers[id] = new char[1145]{};

    ImGui::Text(text);
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint(id, hint, buffers[id], 1145, ImGuiInputTextFlags_ElideLeft);
    if (ImGui::IsItemDeactivatedAfterEdit())
        *pathPtr = buffers[id];
    else if (*pathPtr != buffers[id])
        strcpy_s(buffers[id], 1145, pathPtr->c_str());
}

void StateCharting::ImGuiInputDouble(const char* text, const char* id, double* doublePtr)
{
    if (buffers.find(id) == buffers.end())
        buffers[id] = new char[1145]{};

    ImGui::Text(text);
    ImGui::SetNextItemWidth(-1);
    ImGui::InputDouble(id, doublePtr);
}


void StateCharting::ImGuiInputFloat(const char* text, const char* id, double* floatPtr)
{
    if (buffers.find(id) == buffers.end())
        buffers[id] = new char[1145]{}, sprintf_s(buffers[id], 1145, "%g", *floatPtr);

    ImGui::Text(text);
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText(id, buffers[id], 1145, ImGuiInputTextFlags_CharsDecimal);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        exprtk::expression<double> expression;
        exprtk::parser<double> parser;
        parser.compile((std::string)buffers[id], expression);
        *floatPtr = expression.value();
        sprintf_s(buffers[id], 1145, "%g", *floatPtr);
    }
    if (!ImGui::IsItemActive())
        sprintf_s(buffers[id], 1145, "%g", *floatPtr);
}
