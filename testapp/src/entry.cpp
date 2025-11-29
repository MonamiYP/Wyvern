#include <EntryPoint.hpp>
#include "TestGame.hpp"

Game& createGame() {
    static TestGame game;

    // Configure app
    game.app_config.app_name = "Wyvern Test App";
    game.app_config.window_width = 800;
    game.app_config.window_height = 600;

    return game;
}