#pragma once
#include "State.h"

class StateLiveCharting : public State
{
public:
    void init(Game* _game) override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvent(sf::Event event) override;
    void update() override;
    void render() override;

    static StateLiveCharting* instance() { return &m_stateLiveCharting; }

protected:
    StateLiveCharting() = default;

    bool musicPlayable() const { return game->music.getDuration().asMilliseconds() != 0; }

private:
    static StateLiveCharting m_stateLiveCharting;
    sf::CircleShape planet1, planet2;
    size_t nowTileIndex{}, playerTileIndex{};
    double seconds{}, beat{};
    sf::Clock spareClock;
    double spareClockOffset{};
    std::optional<sf::SoundBuffer> soundBuffer;
    std::optional<sf::Sound> music;
    std::optional<std::vector<double>> samples;
};
