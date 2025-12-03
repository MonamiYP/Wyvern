#pragma once

#include <string>
#include <thread>

#include "core/Logger.hpp"
#include "core/glfw/Window.hpp"
#include "core/glfw/Input.hpp"
#include "core/Clock.hpp"
#include "events/EventTypes.hpp"

#include "renderer/Renderer.hpp"

class Game;
class Window;

struct ApplicationConfig {
    int window_width;
    int window_height;
    std::string app_name;
};

struct ApplicationState {
    std::unique_ptr<Window> window;
    Game* game;
    Clock clock;
    bool is_running;
    int window_width;
    int window_height;
    float last_time;
};

/*
    Manage lifecycle of a game
*/

class Application {
    public:
        static void init(Game* game);
        void run();

        static Application& get() { return *s_instance; }
        Window* getWindow() { return m_state.window.get(); }
        EventDispatcher* getEventDispatcher() { return &m_dispatcher; }

    private:
        Application(Game* game);

        bool onWindowResize(WindowResizeEvent& e);

        static Application* s_instance;
        ApplicationState m_state;
        EventDispatcher m_dispatcher;
};