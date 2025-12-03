#include "renderer/vulkan/VulkanFence.hpp"
#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/Logger.hpp"

void VulkanFence::create(VulkanContext& context, bool create_signaled) {
    m_context = &context;
    is_signaled = create_signaled;

    VkFenceCreateInfo fence_info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    if (is_signaled) fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateFence(m_context->device.getLogicalDevice(), &fence_info, nullptr, &m_fence);
    if (result != VK_SUCCESS) Logger::fatal("Failed to create fence");

    Logger::info("Successfully created fence");
}

void VulkanFence::destroy() {
    vkDestroyFence(m_context->device.getLogicalDevice(), m_fence, nullptr);
    is_signaled = false;
}

bool VulkanFence::wait(uint64_t nanosec) {
    if (!is_signaled) {
        VkResult result = vkWaitForFences(m_context->device.getLogicalDevice(), 1, &m_fence, VK_TRUE, nanosec);
        switch (result) {
            case VK_SUCCESS:
                is_signaled = true;
                return true;
            case VK_TIMEOUT:
                Logger::warn("vk_fence_wait - Timed out");
                break;
            case VK_ERROR_DEVICE_LOST:
                Logger::error("vk_fence_wait - VK_ERROR_DEVICE_LOST.");
                break;
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                Logger::error("vk_fence_wait - VK_ERROR_OUT_OF_HOST_MEMORY.");
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                Logger::error("vk_fence_wait - VK_ERROR_OUT_OF_DEVICE_MEMORY.");
                break;
            default:
                Logger::error("vk_fence_wait - An unknown error has occurred.");
                break;
        }
    } else {
        return true;
    }

    return false;
}

void VulkanFence::reset() {
    if (is_signaled) {
        VkResult result = vkResetFences(m_context->device.getLogicalDevice(), 1, &m_fence);
        is_signaled = false;
    }
}