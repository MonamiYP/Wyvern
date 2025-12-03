#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "renderer/vulkan/VulkanFramebuffer.hpp"
#include "renderer/vulkan/VulkanImage.hpp"

/*
    Vulkan doesn't have default framebuffer, need an infrastructure that owns buffers that will be rendered to --> swapchain
    Basically a queue of images (present images) that are waiting to be presented to the screen (because GPU draws onto an image, not directly onto a screen)
    There are usually 2 or 3 swapchain images:
    - One image is currently being shown
    - One image is available for rendering
    - Another is queued for future presentation

    Every frame you have to acquire an image from the swapchain via vkAcquireImageIndex() which gives index i into swapchain image list. Then you can render into that swapchainImages[i]. To render that image you have to begin command buffer, then renderpass, draw, then end renderpass and command buffer.
*/

struct VulkanContext;

class VulkanSwapchain {
    public:
        void create(uint32_t width, uint32_t height, VulkanContext& context);
        void recreate();
        void destroy();
        VkResult acquireNextImageIndex(VkSemaphore image_available_semaphore, uint32_t* out_image_index);
        void present(VkQueue presentQueue, VkSemaphore signalSemaphores, uint32_t imageIndex);

        void regenerateFramebuffers();

        VkFormat getImageFormat() { return m_imageFormat; }
        VkExtent2D getSwapchainExtent() { return m_swapChainExtent; }
        int getImageCount() { return static_cast<int>(m_images.size()); }
        VulkanFramebuffer& getFrameBuffer(int index) { return m_framebuffers[index]; }

   private:
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
        void createImageViews();

        VulkanContext* m_context;

        VkSwapchainKHR m_swapChain;
        std::vector<VkImage> m_images;
        std::vector<VkImageView> m_imageViews;
        std::vector<VulkanFramebuffer> m_framebuffers;

        VkFormat m_imageFormat;
        VkExtent2D m_swapChainExtent;
        VulkanImage m_depthAttachment;
};