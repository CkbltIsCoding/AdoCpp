#include "Charting.h"
#include "Playing.h"

#include <imgui.h>
#include <ImGuiFileDialog.h>
#include <imgui-SFML.h>

StateCharting StateCharting::m_stateCharting;

void StateCharting::init(Game* game)
{
	m_game = game;
	newLevel();
}

void StateCharting::cleanup()
{
}

void StateCharting::pause()
{
}

void StateCharting::resume()
{
	m_game->activeTileIndex = -1;
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
				m_game->pushState(StatePlaying::instance());
		}
	}
	if (!ImGui::GetIO().WantCaptureMouse)
	{
		if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>())
		{
			static sf::Vector2f d;
			if (mws->wheel == sf::Mouse::Wheel::Vertical) d = { 0, mws->delta * m_game->zoom.y };
			else d = { -mws->delta * m_game->zoom.y, 0 };
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
				|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))
			{
				if (d.x > 0 || d.y > 0) m_game->zoom /= 1.5f;
				else m_game->zoom *= 1.5f;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)
				|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift))
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
					auto mouseCoords = m_game->window.mapPixelToCoords(mbp->position);
					if (m_game->tileSystem[i].isPointInside(mouseCoords))
					{
						m_game->activeTileIndex = i;
						return;
					}
				}
				m_game->activeTileIndex = -1;
				//m_isDragging = true;
				//oPos = mbp->position;
			}
		}
	}
	if (const auto* mm = event.getIf<sf::Event::MouseMoved>())
	{
		//if (m_isDragging)
		//{
		//	auto a = m_game->window.mapPixelToCoords(oPos),
		//		b = m_game->window.mapPixelToCoords(mm->position);
		//	m_game->view.move(a - b);
		//	oPos = mm->position;
		//}
	}
}

void StateCharting::update()
{
	//if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	//	m_isDragging = false;
	float w = m_game->windowSize.x, h = m_game->windowSize.y;
	m_game->view.setSize( { w / (w + h) * 16 * m_game->zoom.x,
		-h / (w + h) * 16 * m_game->zoom.y });
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

	//ImGui::ShowDemoWindow();

	static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse;

	static float barWidth;
	barWidth = ImGui::GetFontSize() * 15;
	ImGui::SetNextWindowSize(ImVec2(barWidth, 0));
	ImGui::SetNextWindowPos(ImVec2(m_game->windowSize.x / 2.f - barWidth / 2.f, 0));
	if (ImGui::Begin("FilenameBar", nullptr, flags))
	{
		std::string filename = m_game->levelPath.filename().string();
		if (filename.empty()) filename = "Untitled";
		if (ImGui::TreeNodeEx("FilenameBarTreeNode", ImGuiTreeNodeFlags_SpanAvailWidth, (filename).c_str()))
		{
			if (ImGui::Button("Open ...", ImVec2(-1, 0)))
			{
				IGFD::FileDialogConfig config;
				config.path = ".";
				config.countSelectionMax = 1;
				config.flags = ImGuiFileDialogFlags_Modal;
				ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".adofai", ImVec4(0.0f, 1.0f, 0.5f, 0.9f));
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose an ADOFAI file", ".adofai", config);
			}
			ImGui::TreePop();
		}
		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				m_game->levelPath = ImGuiFileDialog::Instance()->GetFilePathName();
				m_game->level.clear();
				m_game->level.fromFile(m_game->levelPath.string());
				newLevel();
			}
			ImGuiFileDialog::Instance()->Close();
		}
	} ImGui::End();

	static float leftWidth, leftHeight;
	leftWidth = ImGui::GetFontSize() * 15, leftHeight = ImGui::GetFontSize() * 30;
	ImGui::SetNextWindowSize(ImVec2(leftWidth, leftHeight));
	ImGui::SetNextWindowPos(ImVec2(0, m_game->windowSize.y / 2.f - leftHeight / 2.f));
	if (ImGui::Begin("##LeftSettings", nullptr, flags))
	{
		static float leftSettingsTabContentWidth;
		leftSettingsTabContentWidth = leftWidth / 5 * 4;
		static size_t selectedTab = 0;
		static const std::array<std::string, 7> titles = {
			"Song Settings", "Level Settings", "Track Settings",
			"Background Settings", "Camera Settings", "Miscellaneous Settings",
			"Decorations"
		};
		if (ImGui::BeginChild("##LeftSettings/TabContent", ImVec2(leftSettingsTabContentWidth, 0)))
		{
			ImGui::SetCursorPosX(leftSettingsTabContentWidth / 2
				- ImGui::CalcTextSize(titles[selectedTab].c_str()).x / 2);
			ImGui::Text(titles[selectedTab].c_str());
			switch (selectedTab)
			{
			case 0: // Song
			{
				ImGuiInputFilename("Song Filename:", "##LeftSettings/TabContent/SongSettings/SongFilename", "No files selected", &m_game->level.settings.songFilename);
				ImGuiInputDouble("BPM:", "##LeftSettings/TabContent/SongSettings/BPM", &m_game->level.settings.bpm);
				ImGuiInputDouble("Volume:", "##LeftSettings/TabContent/SongSettings/Volume", &m_game->level.settings.volume);
				ImGuiInputDouble("Offset:", "##LeftSettings/TabContent/SongSettings/Offset", &m_game->level.settings.offset);
				ImGuiInputDouble("Pitch:", "##LeftSettings/TabContent/SongSettings/Pitch", &m_game->level.settings.pitch);
				ImGui::Text("Hitsound:"); // ImGui::SetNextItemWidth(-1);
				ImGuiInputDouble("Hitsound Volume:", "##LeftSettings/TabContent/SongSettings/HitSoundVolume", &m_game->level.settings.hitsoundVolume);
				ImGuiInputDouble("Countdown Ticks:", "##LeftSettings/TabContent/SongSettings/CountdownTicks", &m_game->level.settings.countdownTicks);
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
		if (ImGui::BeginChild("##LeftSettings/TabBtns"))
		{
			if (ImGui::BeginTable("##LeftSettings/TabBtns/Table", 1))
			{
				for (size_t i = 0; i < 7; i++)
				{
					ImGui::TableNextRow(), ImGui::TableNextColumn();
					ImGui::PushID(("##LeftSettings/TabBtns/Table/TabBtn" + std::to_string(i)).c_str());
					if (ImGui::Button(std::to_string(i).c_str(), ImVec2(-1, 0)))
						selectedTab = i;
					ImGui::PopID();
				}
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();

	} ImGui::End();

	auto tile = m_game->activeTileIndex == -1
		? nullptr
		: &m_game->level.tiles[m_game->activeTileIndex];
	static float rightWidth, rightHeight;
	rightWidth = ImGui::GetFontSize() * 15, rightHeight = ImGui::GetFontSize() * 30;
	ImGui::SetNextWindowSize(ImVec2(rightWidth, rightHeight));
	ImGui::SetNextWindowPos(ImVec2(m_game->windowSize.x - rightWidth, m_game->windowSize.y / 2.f - rightHeight / 2.f));
	if (ImGui::Begin("##RightSettings", nullptr, flags))
	{
		static float rightSettingsTabBtnsWidth, rightSettingsTabContentWidth;
		rightSettingsTabBtnsWidth = rightWidth / 5,
			rightSettingsTabContentWidth = rightWidth - rightSettingsTabBtnsWidth;
		static size_t selectedTab = 0;
		if (ImGui::BeginChild("##RightSettings/TabBtns", ImVec2(rightSettingsTabBtnsWidth, 0)))
		{
			if (ImGui::BeginTable("##RightSettings/TabBtns/Table", 1))
			{
				if (tile)
				{
					for (size_t i = 0; i < tile->events.size(); i++)
					{
						ImGui::TableNextRow(), ImGui::TableNextColumn();
						ImGui::PushID(("##RightSettings/TabBtns/Table/TabBtn" + std::to_string(i)).c_str());
						if (ImGui::Button(std::to_string(i).c_str(), ImVec2(-1, 0)))
							selectedTab = i;
						ImGui::PopID();
					}
				}
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::BeginChild("##RightSettings/TabContent"))
		{
			if (tile && !tile->events.empty())
			{
				std::string title = tile->events[selectedTab]->name();
				ImGui::SetCursorPosX(rightSettingsTabContentWidth / 2
					- ImGui::CalcTextSize(title.c_str()).x / 2);
				ImGui::Text(title.c_str());
			}
		}
		ImGui::EndChild();
	} ImGui::End();

	ImGui::SFML::Render(m_game->window);

	m_game->window.setView(m_game->view);
}

void StateCharting::newLevel()
{
	m_game->activeTileIndex = -1;
	m_game->level.parse();
	m_game->level.update();
	m_game->tileSystem.parse();
	m_game->tileSystem.update();
	m_game->musicPath = m_game->levelPath.parent_path().append(m_game->level.settings.songFilename);
	if (!m_game->musicPath.empty())
		m_game->music.openFromFile(m_game->musicPath);;;;;;;;;;;;;;
	m_game->view.setCenter({
		(float)m_game->level.tiles[0].pos.first,
		(float)m_game->level.tiles[0].pos.second });
	m_game->view.setRotation(sf::degrees(0));
	m_game->zoom = { 1.f, 1.f };
}

static std::map<std::string, char*> buffers;
void StateCharting::ImGuiInputFilename(const char* text, const char* id, const char* hint, std::string* pathPtr)
{
	if (buffers.find(id) == buffers.end())
		buffers[id] = new char[1145] {};

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
		buffers[id] = new char[1145] {};

	ImGui::Text(text);
	ImGui::SetNextItemWidth(-1);
	ImGui::InputDouble(id, doublePtr);
}
