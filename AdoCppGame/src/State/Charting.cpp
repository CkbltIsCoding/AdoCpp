#include "Charting.h"
#include "LiveCharting.h"
#include "Playing.h"

#include <AudioProcessing.h>
#include <ImGuiFileDialog.h>
#include <exprtk.hpp>
#include <future>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>

#include <implot.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>

constexpr ImGuiWindowFlags ADOCPPGAME_FLAGS =
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;


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
        parser.compile(buffers[id], expression);
        *doublePtr = expression.value();
        sprintf_s(buffers[id], 1145, "%g", *doublePtr);
    }
    if (!ImGui::IsItemActive())
        sprintf_s(buffers[id], 1145, "%g", *doublePtr);
}


void ImGuiInputFloat(const char* text, const char* id, float* floatPtr)
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
        parser.compile(buffers[id], expression);
        *floatPtr = expression.value();
        sprintf_s(buffers[id], 1145, "%g", *floatPtr);
    }
    if (!ImGui::IsItemActive())
        sprintf_s(buffers[id], 1145, "%g", *floatPtr);
}

void ImGuiInputBool(const char* text, const char* id, bool* boolPtr)
{
    ImGui::Text(text);
    const float w = ImGui::GetColumnWidth();
    ImGui::SetNextItemWidth(w / 2);
    if (ImGui::Button("Enabled"))
        *boolPtr = true;
    ImGui::SameLine();
    ImGui::SetNextItemWidth(w / 2);
    if (ImGui::Button("Disabled"))
        *boolPtr = false;
}

static std::map<std::string, float*> colorBuffers;
void ImGuiInputColor(const char* text, const char* id, AdoCpp::Color* colorPtr)
{
    if (!colorBuffers.contains(id))
        colorBuffers[id] = new float[4];
    ImGui::Text(text);
    ImGui::SetNextItemWidth(-1);
    ImGui::ColorEdit4(text, colorBuffers[id]);
    if (ImGui::IsItemEdited())
        *colorPtr = AdoCpp::Color(
            static_cast<uint8_t>(colorBuffers[id][0] * 255), static_cast<uint8_t>(colorBuffers[id][1] * 255),
            static_cast<uint8_t>(colorBuffers[id][2] * 255), static_cast<uint8_t>(colorBuffers[id][3] * 255));
    if (!ImGui::IsItemActive())
    {
        colorBuffers[id][0] = static_cast<float>(colorPtr->r) / 255.f;
        colorBuffers[id][1] = static_cast<float>(colorPtr->g) / 255.f;
        colorBuffers[id][2] = static_cast<float>(colorPtr->b) / 255.f;
        colorBuffers[id][3] = static_cast<float>(colorPtr->a) / 255.f;
    }
}

void ImGuiInputUint32(const char* text, const char* id, uint32_t* uint32Ptr)
{
    if (!buffers.contains(id))
        buffers[id] = new char[1145]{}, sprintf_s(buffers[id], 1145, "%u", *uint32Ptr);

    ImGui::Text(text);
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText(id, buffers[id], 1145, ImGuiInputTextFlags_CharsDecimal);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        *uint32Ptr = std::stoul(buffers[id]);
        sprintf_s(buffers[id], 1145, "%u", *uint32Ptr);
    }
    if (!ImGui::IsItemActive())
        sprintf_s(buffers[id], 1145, "%u", *uint32Ptr);
}


StateCharting StateCharting::m_stateCharting;

void StateCharting::init(Game* l_game)
{
    game = l_game;
    newLevel();
    game->tileSystem.setTilePlaceMode(0);
}

void StateCharting::cleanup() {}

void StateCharting::pause()
{
}

void StateCharting::resume()
{
    game->tileSystem.setTilePlaceMode(0);
    game->activeTileIndex = std::nullopt;
    game->level.update();
    game->tileSystem.update();
    game->view.setRotation(sf::degrees(0));
}

void StateCharting::handleEvent(const sf::Event event)
{
    using enum sf::Keyboard::Key;
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == F12)
                game->autoplay = !game->autoplay;
            else if (keyPressed->code == Space)
                game->level.initCamera(), game->pushState(StatePlaying::instance());
            else if (keyPressed->code == Grave)
                game->pushState(LiveCharting::instance());
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

void StateCharting::update()
{
    const auto w = static_cast<float>(game->windowSize.x), h = static_cast<float>(game->windowSize.y);
    game->view.setSize({w / (w + h) * 16 * game->zoom.x, -h / (w + h) * 16 * game->zoom.y});
    game->tileSystem.setActiveTileIndex(game->activeTileIndex);
    game->tileSystem.update();
}

void StateCharting::render()
{
    // render the world
    game->window.setView(game->view);
    game->window.draw(game->tileSystem);

    // render the GUI
    sf::View defaultView = game->window.getDefaultView();
    defaultView.setSize(sf::Vector2f(game->windowSize));
    defaultView.setCenter(sf::Vector2f(game->windowSize) / 2.f);
    game->window.setView(defaultView);

#ifndef NDEBUG
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();
#endif // NDEBUG

    renderFilenameBar();
    renderSettings();
    renderEventSettings();
}
void StateCharting::renderFilenameBar()
{
    const float barWidth = ImGui::GetFontSize() * 15;
    ImGui::SetNextWindowSize(ImVec2(barWidth, 0));
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(game->windowSize.x) / 2.f - barWidth / 2.f, 0));
    if (ImGui::Begin("FilenameBar", nullptr, ADOCPPGAME_FLAGS))
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        static std::future<std::filesystem::path> future;
        static float progress;
        static std::optional<std::exception> exception;
        std::string filename = game->levelPath.filename().string();
        if (filename.empty())
            filename = "Untitled";
        if (ImGui::TreeNodeEx("FilenameBarTreeNode", ImGuiTreeNodeFlags_SpanAvailWidth, filename.c_str()))
        {
            if (ImGui::Button("Open ...", ImVec2(-1, 0)))
            {
                IGFD::FileDialogConfig config;
                config.path = ".";
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose an ADOFAI file", ".adofai", config);
            }
            if (ImGui::Button("Save as ...", ImVec2(-1, 0)))
            {
                IGFD::FileDialogConfig config;
                config.path = game->origMusicPath.parent_path().string();
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Save the ADOFAI file", ".adofai", config);
            }
            if (ImGui::Button(addedHitsound ? "Re-add hitsound" : "Add hitsound", ImVec2(-1, 0)))
            {
                future =
                    std::async(std::launch::async, addHitsound, game->origMusicPath, game->level.getTiles(), &progress);
                ImGui::OpenPopup("Adding hitsound...");
            }
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
                    if (!exception)
                    {
                        if (progress == -1)
                            ImGui::Text("Loading...");
                        else if (progress == 2)
                            ImGui::Text("Saving...");
                        else
                            ImGui::Text("Adding hitsound...");
                        ImGui::ProgressBar(std::max(0.f, std::min(1.f, progress)), ImVec2(-1, 0));
                    }
                    else
                    {
                        ImGui::Text("An error occurred. %s", exception->what());
                        if (ImGui::Button("Close"))
                            ImGui::CloseCurrentPopup();
                    }

                    if (future.valid() && future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            game->musicPath = future.get();
                            addedHitsound = true;
                            if (!game->musicPath.empty() && !game->music.openFromFile(game->musicPath))
                                throw std::runtime_error("Failed to load music");
                            ImGui::CloseCurrentPopup();
                        }
                        catch (const std::exception& ex)
                        {
                            exception = ex;
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
        if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                const auto path = ImGuiFileDialog::Instance()->GetFilePathName();
                std::ofstream ofs(path, std::ios::binary);
                rapidjson::Document doc = game->level.intoJson();
                rapidjson::OStreamWrapper osw(ofs);
                rapidjson::EncodedOutputStream<rapidjson::UTF8<>, rapidjson::OStreamWrapper> eos(osw, true);
                rapidjson::PrettyWriter writer(eos);
                doc.Accept(writer);
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
}
void StateCharting::renderSettings() const
{
    const float width = ImGui::GetFontSize() * 15, height = ImGui::GetFontSize() * 30;
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(game->windowSize.y) / 2.f - height / 2.f));
    if (ImGui::Begin("LeftSettings", nullptr, ADOCPPGAME_FLAGS))
    {
        static float leftSettingsTabContentWidth;
        leftSettingsTabContentWidth = width / 5 * 4;
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
            case 0:
                renderSSong();
                break;
            case 1:
                renderSLevel();
                break;
            case 2:
                renderSTrack();
                break;
            case 3:
                renderSBackground();
                break;
            case 4:
                renderSCamera();
                break;
            case 5:
                renderSMiscellaneous();
                break;
            case 6:
                renderSDecorations();
                break;
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
}
void StateCharting::renderEventSettings() const
{
    const bool windowOpen = game->activeTileIndex.has_value();
    const float width = ImGui::GetFontSize() * 15, height = ImGui::GetFontSize() * 30;
    if (windowOpen)
    {
        const auto& tile = game->level.getTiles()[*game->activeTileIndex];
        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(game->windowSize.x) - width,
                                       static_cast<float>(game->windowSize.y) / 2.f - height / 2.f));
        if (ImGui::Begin("RightSettings", nullptr, ADOCPPGAME_FLAGS))
        {
            static float rightSettingsTabBtnsWidth, rightSettingsTabContentWidth;
            rightSettingsTabBtnsWidth = width / 5, rightSettingsTabContentWidth = width - rightSettingsTabBtnsWidth;
            static std::optional<size_t> tileIndex;
            static size_t selectedTab = 0;
            if (game->activeTileIndex != tileIndex)
                tileIndex = game->activeTileIndex, selectedTab = 0;
            if (ImGui::BeginChild("RightSettings/TabBtns", ImVec2(rightSettingsTabBtnsWidth, 0)))
            {
                if (ImGui::BeginTable("RightSettings/TabBtns/Table", 1))
                {
                    for (size_t i = 0; i < tile.events.size(); i++)
                    {
                        ImGui::TableNextRow(), ImGui::TableNextColumn();
                        ImGui::PushID(("RightSettings/TabBtns/Table/TabBtn" + std::to_string(i)).c_str());
                        if (ImGui::Button(tile.events[i]->name(), ImVec2(-1, 0)))
                            selectedTab = i;
                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::EndChild();
            ImGui::SameLine();
            if (ImGui::BeginChild("RightSettings/TabContent"))
            {
                if (!tile.events.empty())
                {
                    const char* title = tile.events[selectedTab]->name();
                    ImGui::SetCursorPosX(rightSettingsTabContentWidth / 2 - ImGui::CalcTextSize(title).x / 2);
                    ImGui::Text(title);
                    auto event = tile.events[selectedTab];
                    if (const auto twirl = std::dynamic_pointer_cast<AdoCpp::Event::GamePlay::Twirl>(event))
                    {
                    }
                    if (const auto setSpeed = std::dynamic_pointer_cast<AdoCpp::Event::GamePlay::SetSpeed>(event))
                    {
                    }
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
void StateCharting::renderSSong() const
{
    auto& settings = game->level.getSettings();
#define ADOCPPGAME_P "LeftSettings/TabContent/SongSettings/"
    ImGuiInputFilename("Song Filename", ADOCPPGAME_P "SongFilename", "No files selected", &settings.songFilename);
    ImGuiInputDouble("BPM", ADOCPPGAME_P "BPM", &settings.bpm);
    ImGuiInputDouble("Volume", ADOCPPGAME_P "Volume", &settings.volume);
    ImGuiInputDouble("Offset", ADOCPPGAME_P "Offset", &settings.offset);
    ImGuiInputDouble("Pitch", ADOCPPGAME_P "Pitch", &settings.pitch);
    {
        static int selected;
        selected = static_cast<int>(settings.hitsound);
        ImGui::Text("Hitsound");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##" ADOCPPGAME_P "/Hitsound", AdoCpp::cstrHitsound[selected]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(AdoCpp::cstrHitsound); n++)
            {
                const bool is_selected = selected == n;
                if (ImGui::Selectable(AdoCpp::cstrHitsound[n], is_selected))
                    selected = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        settings.hitsound = static_cast<AdoCpp::Hitsound>(selected);
    }
    ImGuiInputDouble("Hitsound Volume", ADOCPPGAME_P "HitSoundVolume", &settings.hitsoundVolume);
    ImGuiInputDouble("Countdown Ticks", ADOCPPGAME_P "CountdownTicks", &settings.countdownTicks);
#undef ADOCPPGAME_P
}
void StateCharting::renderSLevel() const
{
    auto& settings = game->level.getSettings();
#define ADOCPPGAME_P "LeftSettings/TabContent/LevelSettings/"
    ImGuiInputStdString("Artist", ADOCPPGAME_P "Artist", &settings.artist);
    ImGuiInputStdString("Song", ADOCPPGAME_P "Song", &settings.song);
    ImGuiInputStdString("Author", ADOCPPGAME_P "Author", &settings.author);
    ImGuiInputBool("Separate countdown time", ADOCPPGAME_P "SeparateCountdownTime", &settings.separateCountdownTime);
#undef ADOCPPGAME_P
}
void StateCharting::renderSTrack() const
{
    auto& settings = game->level.getSettings();
#define ADOCPPGAME_P "LeftSettings/TabContent/TrackSettings/"
    {
        static int selected;
        selected = static_cast<int>(settings.trackColorType);
        ImGui::Text("Track color type");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##" ADOCPPGAME_P "/TrackColorType", AdoCpp::cstrTrackColorType[selected]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(AdoCpp::cstrTrackColorType); n++)
            {
                const bool is_selected = selected == n;
                if (ImGui::Selectable(AdoCpp::cstrTrackColorType[n], is_selected))
                    selected = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        settings.trackColorType = static_cast<AdoCpp::TrackColorType>(selected);
    }

    ImGuiInputColor("Track color", ADOCPPGAME_P "TrackColor", &settings.trackColor);
    ImGuiInputColor("Secondary track color", ADOCPPGAME_P "SecondaryTrackColor", &settings.trackColor);
    ImGuiInputDouble("Track color animation duration", ADOCPPGAME_P "TrackColorAnimDuration",
                     &settings.trackColorAnimDuration);
    {
        static int selected;
        selected = static_cast<int>(settings.trackColorPulse) + 1;
        ImGui::Text("Track color pulse");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##" ADOCPPGAME_P "/TrackColorPulse", AdoCpp::cstrTrackColorPulse[selected]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(AdoCpp::cstrTrackColorPulse); n++)
            {
                const bool is_selected = selected == n;
                if (ImGui::Selectable(AdoCpp::cstrTrackColorPulse[n], is_selected))
                    selected = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        settings.trackColorPulse = static_cast<AdoCpp::TrackColorPulse>(selected - 1);
    }
    ImGuiInputUint32("Track pulse length", ADOCPPGAME_P "TrackPulseLength", &settings.trackPulseLength);
    {
        static int selected;
        selected = static_cast<int>(settings.trackStyle);
        ImGui::Text("Track style");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##" ADOCPPGAME_P "/TrackStyle", AdoCpp::cstrTrackStyle[selected]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(AdoCpp::cstrTrackStyle); n++)
            {
                const bool is_selected = selected == n;
                if (ImGui::Selectable(AdoCpp::cstrTrackStyle[n], is_selected))
                    selected = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        settings.trackStyle = static_cast<AdoCpp::TrackStyle>(selected);
    }
    {
        static int selected;
        selected = static_cast<int>(settings.trackAnimation);
        ImGui::Text("Track animation");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##" ADOCPPGAME_P "/TrackAnimation", AdoCpp::cstrTrackAnimation[selected]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(AdoCpp::cstrTrackAnimation); n++)
            {
                const bool is_selected = selected == n;
                if (ImGui::Selectable(AdoCpp::cstrTrackAnimation[n], is_selected))
                    selected = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        settings.trackAnimation = static_cast<AdoCpp::TrackAnimation>(selected);
    }
    ImGuiInputDouble("Beats ahead", ADOCPPGAME_P "BeatsAhead", &settings.beatsAhead);
    {
        static int selected;
        selected = static_cast<int>(settings.trackDisappearAnimation);
        ImGui::Text("Track disappear animation");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##" ADOCPPGAME_P "/TrackDisappearAnimation",
                              AdoCpp::cstrTrackDisappearAnimation[selected]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(AdoCpp::cstrTrackDisappearAnimation); n++)
            {
                const bool is_selected = selected == n;
                if (ImGui::Selectable(AdoCpp::cstrTrackDisappearAnimation[n], is_selected))
                    selected = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        settings.trackDisappearAnimation = static_cast<AdoCpp::TrackDisappearAnimation>(selected);
    }
    ImGuiInputDouble("Beats behind", ADOCPPGAME_P "BeatsBehind", &settings.beatsBehind);
#undef ADOCPPGAME_P
}
void StateCharting::renderSBackground() const
{
    auto& settings = game->level.getSettings();
#define ADOCPPGAME_P "LeftSettings/TabContent/BackgroundSettings/"
    ImGuiInputColor("Background color", ADOCPPGAME_P "BackgroundColor", &settings.backgroundColor);
#undef ADOCPPGAME_P
}
void StateCharting::renderSCamera() const
{
    auto& settings = game->level.getSettings();
#define ADOCPPGAME_P "LeftSettings/TabContent/CameraSettings/"
    {
        static int selected;
        selected = static_cast<int>(settings.relativeTo);
        ImGui::Text("Relative to");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##" ADOCPPGAME_P "/RelativeTo", AdoCpp::cstrRelativeToCamera[selected]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(AdoCpp::cstrRelativeToCamera); n++)
            {
                const bool is_selected = selected == n;
                if (ImGui::Selectable(AdoCpp::cstrRelativeToCamera[n], is_selected))
                    selected = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        settings.relativeTo = static_cast<AdoCpp::RelativeToCamera>(selected);
    }
#undef ADOCPPGAME_P
}
void StateCharting::renderSMiscellaneous() const
{
    auto& settings = game->level.getSettings();
#define ADOCPPGAME_P "LeftSettings/TabContent/MiscellaneousSettings/"
    ImGuiInputBool("Stick to floors", ADOCPPGAME_P "StickToFloors", &settings.stickToFloors);
#undef ADOCPPGAME_P
}
void StateCharting::renderSDecorations() const
{
    auto& settings = game->level.getSettings(); // TODO MAYBE I WILL NEVER DO THIS owo
#define ADOCPPGAME_P "LeftSettings/TabContent/Decorations/"
#undef ADOCPPGAME_P
}

void StateCharting::newLevel()
{
    addedHitsound = false;
    game->activeTileIndex = std::nullopt;
    game->level.parse();
    game->level.update();
    game->tileSystem.parse();
    game->tileSystem.update();
    game->origMusicPath = game->musicPath =
        game->levelPath.parent_path().append(game->level.getSettings().songFilename);
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
