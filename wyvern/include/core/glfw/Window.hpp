#pragma once

#include <string>
#include <vulkan/vulkan.h> // In the future want to remove this from here
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
        void getFramebufferSize(int& width, int& height) const { glfwGetFramebufferSize(m_window, &width, &height); }

        void update();

        const char** getGLFWExtensions(uint32_t* count) { return glfwGetRequiredInstanceExtensions(count); }

        // Eventualy move this somewhere else so Window doesn't need to know about which API is being used but for now I'm only using Vulkan so this is fine
        void createVulkanSurface(VkInstance& instance, VkSurfaceKHR& surface);

    private:
        void init(const WindowConfig& config);
        void setupCallbacks();
        void shutdown();

        GLFWwindow* m_window;
        WindowConfig m_config;
};