#include "renderer/vulkan/VulkanSwapchain.hpp"

#include "renderer/vulkan/VulkanBackend.hpp"

#include "core/Logger.hpp"
#include "core/glfw/Window.hpp"

void VulkanSwapchain::create(uint32_t width, uint32_t height, VulkanContext& context) {
    m_context = &context;
    
    SwapChainSupportDetails swapchainSupport = m_context->device.getSwapchainSupportDetails();
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities, width, height);

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
        imageCount = swapchainSupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    create_info.surface = m_context->surface;
    create_info.minImageCount = imageCount;
    create_info.imageFormat = surfaceFormat.format;
    create_info.imageColorSpace = surfaceFormat.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Specify how to handle swap chain images that are used across multiple queue families
    // e.g. Draw on images in swap chain from graphics queue then transfer to presentation queue
    QueueFamilyIndices indices = m_context->device.getQueueFamilyIndices();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    if (indices.graphicsFamily != indices.presentFamily) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Image can be used across queue families without explicit ownership transfers
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Image owned by one queue family at a time, ownership must be explicitly transferred
        create_info.queueFamilyIndexCount = 0; // Optional
        create_info.pQueueFamilyIndices = nullptr; // Optional
    }

    create_info.preTransform = swapchainSupport.capabilities.currentTransform; // No transform
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Usually want to ignore alpha channel
    create_info.presentMode = presentMode;
    create_info.clipped = VK_TRUE; // Don't care about obscured pixels
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(m_context->device.getLogicalDevice(), &create_info, nullptr, &m_swapChain);
    if (result != VK_SUCCESS) Logger::fatal("Failed to create swap chain!");

    // Retrieve handles of 'VkImage's in swap chain
    vkGetSwapchainImagesKHR(m_context->device.getLogicalDevice(), m_swapChain, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_context->device.getLogicalDevice(), m_swapChain, &imageCount, m_images.data());

    m_imageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
    Logger::info("Successfully created swapchain");

    createImageViews();

    // Create depth image
    m_depthAttachment.create(*m_context, VK_IMAGE_TYPE_2D, m_swapChainExtent.width, m_swapChainExtent.height, m_context->device.getDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void VulkanSwapchain::recreate() {
    destroy();

    int w, h;
    m_context->window->getFramebufferSize(w, h);

    m_context->framebuffer_width = w;
    m_context->framebuffer_height = h;

    create(w, h, *m_context);
}

void VulkanSwapchain::destroy() {
    vkDeviceWaitIdle(m_context->device.getLogicalDevice());
    for (int i = 0; i < getImageCount(); i++) getFrameBuffer(i).destroy();

    m_depthAttachment.destroy();
    
    for (auto view : m_imageViews) {
        vkDestroyImageView(m_context->device.getLogicalDevice(), view, nullptr);
    }

    vkDestroySwapchainKHR(m_context->device.getLogicalDevice(), m_swapChain, nullptr);
}

VkResult VulkanSwapchain::acquireNextImageIndex(VkSemaphore image_available_semaphore, uint32_t* out_image_index) {
    VkResult result = vkAcquireNextImageKHR(m_context->device.getLogicalDevice(), m_swapChain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE, out_image_index);
    return result;
}

void VulkanSwapchain::present(VkQueue presentQueue, VkSemaphore signalSemaphores, uint32_t imageIndex) {
    /*
        Return image back to the swapchain for presentation
    */
    VkPresentInfoKHR present_info = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &signalSemaphores;
    VkSwapchainKHR swapChains[] = { m_swapChain };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapChains;
    present_info.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(presentQueue, &present_info);
    if (result != VK_SUCCESS) Logger::fatal("Failed to present swap chain image");
}

VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    /*
        VkSurfaceFormatKHR contains 'format' and 'colorSpace' members
        - 'format': color channels and type
        - 'colorSpace': indicate if SRGB color space is supported or not, SRGB results in more accurate perceived colors
    */
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) return availableFormat;
    }

    return availableFormats[0]; // Issok to settle with first available format if ideal is not available
}

VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    /*
        4 possible presentation modes available in Vulkan:
        - VK_PRESENT_MODE_IMMEDIATE_KHR: images transferred to screen right away, can result in tearing
        - VK_PRESENT_MODE_FIFO_KHR: display takes image from front of queue, rendered images at the bakc of queue, program wait if queue is full (pretty much vsync)
        - VK_PRESENT_MODE_FIFO_RELAXED_KHR: same as previous, but if queue is empty, image is transferred right away instead of waiting, may result in tearing
        - VK_PRESENT_MODE_MAILBOX_KHR: same as second, but if queue is full, old images replaced with new ones
    */

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width,
    uint32_t height) {
    VkExtent2D actualExtent = { width, height };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}

void VulkanSwapchain::createImageViews() {
    m_imageViews.resize(m_images.size());

    for (size_t i = 0; i < m_images.size(); i++) {
        VkImageViewCreateInfo create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        create_info.image = m_images[i];

        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = m_imageFormat;

        // Allows swizzling of color channels or map constant values. These are default.
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Describe image purpose and which parts of the image should be accessed
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(m_context->device.getLogicalDevice(), &create_info, nullptr, &m_imageViews[i]);
        if (result != VK_SUCCESS) Logger::fatal("Failed to create image views");
    }
}

void VulkanSwapchain::regenerateFramebuffers() {
    m_framebuffers.resize(m_images.size());

    for (int i = 0; i < m_framebuffers.size(); i++) {
        std::vector<VkImageView> imageViews = { m_imageViews[i], m_depthAttachment.getImageView() };

        m_framebuffers[i].create(*m_context, m_context->framebuffer_width, m_context->framebuffer_height, imageViews);
    }
}