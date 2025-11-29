#pragma once

#include <Game.hpp>
#include <core/Logger.hpp>

class TestGame : public Game {
    void init() override;
    void update(float deltaTime) override;
    void render(float deltaTime) override;
};