#pragma once

#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "Game.hpp"

extern Game& createGame(); // This will be implemented by the game itself outside of engine

/*
    This is the main entry point of the application
    Engine controls the flow
*/
int main() {
    Logger::info("Starting Wyvern Engine...");

    // Request game instance from application
    Game& game = createGame();

    Application::init(&game);
    Application& app = Application::get();
    app.run();

    Logger::info("Shutting down Wyvern Engine...");
    return 0;
}