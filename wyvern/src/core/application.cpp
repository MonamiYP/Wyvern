#include "core/Application.hpp"
#include "Game.hpp"

Application* Application::s_instance = nullptr;

Application::Application(Game* game) {
    m_state.game = game;

    WindowConfig config;
    config.name = game->app_config.app_name;
    config.width = game->app_config.window_width;
    config.height = game->app_config.window_height;

    m_state.window = std::make_unique<Window>(config);

    m_state.game->init();
    m_state.is_running = true;

    m_dispatcher.registerListener<WindowCloseEvent>(
        [this](Event&) -> bool {
            Logger::debug("Close window...");
            m_state.is_running = false;
            return true;
        }
    );
}

void Application::init(Game* game) {
    if (s_instance) {
        return;
    }

    s_instance = new Application(game);
}

void Application::run() {
    const double target_frame_time = 1.0 / 60.0; // 60 FPS
    double run_time = 0;
    Clock::start();

    while (m_state.is_running) {
        double frame_start = Clock::getTimeSinceStart();
        float dt = Clock::getDeltaTime();

        m_state.window->update();
        m_state.game->update(dt);
        m_state.game->render(dt);

        double frame_end = Clock::getTimeSinceStart();
        double elapsed = frame_end - frame_start;
        run_time += elapsed;

        bool limit_frames = false;
        if (elapsed < target_frame_time && limit_frames) {
            double sleepTime = target_frame_time - elapsed;
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }
}