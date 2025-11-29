#pragma once

#include "core/Application.hpp"

/*
    This is the base app for all games created using this engine
    Defines interface for a game
*/

class Game {
    public:
        ApplicationConfig app_config;

        // Each game will override these
        virtual void init() = 0;
        virtual void update(float deltaTime) = 0;
        virtual void render(float deltaTime) = 0;
};