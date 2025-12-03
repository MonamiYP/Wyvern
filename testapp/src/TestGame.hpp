#pragma once

#include <Game.hpp>
#include <core/Logger.hpp>

class TestGame : public Game {
    void init() override;
    void update(float deltaTime) override;
    void render(float deltaTime) override;
    void onWindowResize(uint16_t width, uint16_t height) override;
};