#pragma once

#include <string>
#include <GLFW/glfw3.h>

#include "core/Application.hpp"
#include "core/glfw/Input.hpp"

struct WindowConfig {
    std::string name;
    unsigned int width;
    unsigned int height;
};

class Window {
    public:
        Window(const WindowConfig& config) { init(config); setupCallbacks(); }
        ~Window() { shutdown(); }

        unsigned int getWidth() const { return m_config.width; }
        unsigned int getHeight() const { return m_config.height; }
        GLFWwindow* getNativeWindow() const { return m_window; }

        void update();

    private:
        void init(const WindowConfig& config);
        void setupCallbacks();
        void shutdown();

        GLFWwindow* m_window;
        WindowConfig m_config;
};