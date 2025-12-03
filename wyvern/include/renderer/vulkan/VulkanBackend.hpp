#pragma once

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>

#include "renderer/vulkan/VulkanDevice.hpp"
#include "renderer/vulkan/VulkanSwapchain.hpp"
#include "renderer/vulkan/VulkanRenderpass.hpp"
#include "renderer/vulkan/VulkanFence.hpp"

class Window;

struct RenderPacket {
    float deltaTime;
};

struct VulkanContext {
    Window* window;

    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    # if defined(_DEBUG)
        VkDebugUtilsMessengerEXT debugMessenger;
    # endif
    std::vector<const char*> layers;

    VulkanDevice device;
    VulkanSwapchain swapchain;
    VulkanRenderpass renderpass;

    std::vector<VulkanCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> image_acquire_semaphores; // Ensure it doesn't start rendering until the acquired image is actually available
    std::vector<VkSemaphore> queue_submit_semaphores; // Present image until after rendering has completed
    std::vector<VulkanFence> in_flight_fences; // Fence per frame in flight         
    std::vector<VulkanFence*> images_in_flight; // Pointer per swapchain image

    unsigned int framebuffer_width;
    unsigned int framebuffer_height;
    bool window_resized = false;

    /*
        We want to allow multiple frames to be 'in-flight' at once so we can render one frame while recording the next frame. If CPU finishes early it can wait for GPU to finish rendering work before submitting more work.
        This means we also need to duplicate any resource that is accessed and modified during rendering, so things like command buffers, semaphores and fences
        Default is that 2 in flight frames is enough
    */
    unsigned int max_frames_in_flight = 2;
    unsigned int current_frame = 0;
    unsigned int image_index;
};

class VulkanBackend {
    public:
        void init(const char* appName, Window* window);
        void shutdown();
        void drawFrame(float dt) {
            if (beginFrame(dt)) endFrame(dt);
        }

        void onWindowResize(int width, int height);

    private:
        VulkanContext m_context;

        bool beginFrame(float dt);
        void endFrame(float dt);

        void createInstance(const char* appName);
        void createDebugCallback();

        void createCommandBuffers();
        void createSyncObjects();
        void recreateSwapchain();

        void cleanupSyncObjects();

        std::vector<const char*> getRequiredInstanceExtensions();
        std::vector<const char*> getLayers();
};