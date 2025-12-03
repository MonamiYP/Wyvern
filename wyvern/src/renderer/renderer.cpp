#include "renderer/Renderer.hpp"

#include "core/Logger.hpp"

VulkanBackend Renderer::s_backend;

void Renderer::init(const char* appName, Window* window) {
    s_backend.init(appName, window);
}

void Renderer::shutdown() {
    s_backend.shutdown();
}

void Renderer::drawFrame(RenderPacket& renderPacket) {
    s_backend.drawFrame(renderPacket.deltaTime);
}

void Renderer::onWindowResize(u_int16_t width, u_int16_t height) {
    s_backend.onWindowResize(width, height);
}