#pragma once

#include "State.h"
#include "AssetManager.h"

class StateCharting : public State
{
public:
	void init(Game* l_game);
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
};