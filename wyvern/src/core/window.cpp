#include "core/glfw/Window.hpp"
#include "core/Logger.hpp"

void Window::init(const WindowConfig& config) {
    m_config = config;

    // setup GLFW
    if (!glfwInit()) {
        Logger::error("Failed to initialise GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    m_window = glfwCreateWindow(m_config.width, m_config.height, m_config.name.c_str(), NULL, NULL);
    if (!m_window) {
        Logger::error("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

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

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        Input::processKey(key, action);
    });

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        WindowResizeEvent e(width, height);
        Application::get().getEventDispatcher()->dispatch(e);
    });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
        WindowCloseEvent e;
        Application::get().getEventDispatcher()->dispatch(e);
    });
}

#include <iostream>

void Window::createVulkanSurface(VkInstance& instance, VkSurfaceKHR& surface) {
    VkResult result = glfwCreateWindowSurface(instance, m_window, nullptr, &surface);
    if (result != VK_SUCCESS) throw std::runtime_error("Failed to create Vulkan surface");
}