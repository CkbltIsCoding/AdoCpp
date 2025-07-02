#pragma once

#include "State.h"
#include "AssetManager.h"

class StateCharting : public State
{
public:
	void init(Game* game);
	void cleanup();

	void pause();
	void resume();

	void handleEvent(sf::Event event);
	void update();
	void render();

	void newLevel();

	static StateCharting* instance()
	{
		return &m_stateCharting;
	}

protected:
	StateCharting() {}

private:
	static StateCharting m_stateCharting;
	bool addedHitsound;

	void ImGuiInputFilename(const char* text, const char* id, const char* hint, std::string* pathPtr);
	void ImGuiInputDouble(const char* text, const char* id, double* doublePtr);
	void ImGuiInputFloat(const char* text, const char* id, double* floatPtr);
};