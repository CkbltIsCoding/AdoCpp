#pragma once

#include "Game.h"

class State
{
public:
    virtual ~State() = default;
    virtual void init(Game* game) = 0;
    virtual void cleanup() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual void handleEvent(sf::Event event) = 0;
    virtual void update() = 0;
    virtual void render() = 0;

    void changeState(State* state) const { game->changeState(state); }

protected:
    State() { game = nullptr; }
    Game* game;
};
