#pragma once

#include "HitErrorMeter.h"
#include "HitText.h"
#include "KeyViewer.h"
#include "State.h"

class StatePlaying final : public State
{
public:
    void init(Game* _game) override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvent(sf::Event event) override;
    void update() override;
    void render() override;

    static StatePlaying* instance() { return &m_statePlaying; }

protected:
    StatePlaying() = default;

    bool musicPlayable() const { return game->music.getDuration().asMilliseconds() != 0; }

private:
    static StatePlaying m_statePlaying;
    sf::CircleShape planet1, planet2;
    size_t nowTileIndex{}, playerTileIndex{};
    sf::Font fontHts{"assets/font/Maplestory OTF Bold.otf"};
    HitTextSystem hitTextSystem{fontHts};
    HitErrorMeterSystem hitErrorMeterSystem;
    KeyViewerSystem keyViewerSystem;
    int keyInputCnt{};
    double seconds{}, beat{};
    sf::Clock spareClock;
    double spareClockOffset{};
    bool waiting{};
    bool isMusicPlayed{};
};
