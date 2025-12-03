#pragma once

#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/glfw/Window.hpp"

class Renderer {
    public:
        static void init(const char* appName, Window* window);
        static void shutdown();

        static void drawFrame(RenderPacket& renderPacket);
        static void onWindowResize(u_int16_t width, u_int16_t height);
    
    private:
        static VulkanBackend s_backend;
};