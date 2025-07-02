#pragma once

#include "State.h"
#include "AssetManager.h"
#include "HitErrorMeter.h"
#include "HitText.h"
#include "KeyViewer.h"

class StatePlaying : public State
{
public:
	void init(Game* game);
	void cleanup();

	void pause();
	void resume();

	void handleEvent(sf::Event event);
	void update();
	void render();

	static StatePlaying* instance()
	{
		return &m_statePlaying;
	}

protected:
	StatePlaying() {}

	bool musicPlayable()
	{
		return m_game->music.getDuration().asMilliseconds() != 0;
	}

private:
	static StatePlaying m_statePlaying;
	sf::CircleShape planet1, planet2;
	size_t nowTileIndex, playerTileIndex;
	HitTextSystem hitTextSystem{AssetManager::GetFont("Maplestory OTF Bold.otf")};
	HitErrorMeterSystem hitErrorMeterSystem;
	KeyViewerSystem keyViewerSystem;
	int keyInputCnt;
	double seconds, beat;
	sf::Clock spareClock;
	double spareClockOffset;
	bool waiting;
	bool isMusicPlayed;
};
