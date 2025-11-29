#include "core/glfw/Window.hpp"
#include "core/Logger.hpp"

void Window::init(const WindowConfig& config) {
    m_config = config;

    // setup GLFW
    if (!glfwInit()) {
        Logger::error("Failed to initialise GLFW");
        return;
    }

    m_window = glfwCreateWindow(m_config.width, m_config.height, m_config.name.c_str(), NULL, NULL);
    if (!m_window) {
        Logger::error("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    Logger::info("Window created: %s (%dx%d)", m_config.name.c_str(), m_config.width, m_config.height);
}

void Window::update() {
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
 
void Window::shutdown() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
    Logger::info("Window destroyed");
}

void Window::setupCallbacks() {
    glfwSetWindowUserPointer(m_window, this);

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
        WindowCloseEvent e;
        Application::get().getEventDispatcher()->dispatch(e);
    });
}