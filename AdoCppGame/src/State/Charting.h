#pragma once

#include "State.h"

class StateCharting final : public State
{
public:
	void init(Game* l_game) override;
	void cleanup() override;

	void pause() override;
	void resume() override;

	void handleEvent(sf::Event event) override;
	void update() override;
	void render() override;
    void renderFilenameBar();
    void renderSettings() const;
    void renderEventSettings() const;
    void renderSSong() const;
    void renderSLevel() const;
    void renderSTrack() const;
    void renderSBackground() const;
    void renderSCamera() const;
    void renderSMiscellaneous() const;
    void renderSDecorations() const;

	void newLevel();

	static StateCharting* instance()
	{
		return &m_stateCharting;
	}

protected:
	StateCharting() = default;

private:
	static StateCharting m_stateCharting;
	bool addedHitsound{};
};