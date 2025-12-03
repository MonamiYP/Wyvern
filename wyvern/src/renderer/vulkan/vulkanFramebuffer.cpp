#include "renderer/vulkan/VulkanFramebuffer.hpp"

#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/Logger.hpp"

void VulkanFramebuffer::create(VulkanContext& context, uint32_t width, uint32_t height, const std::vector<VkImageView>& imageViews) {
    m_context = &context;

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_context->renderpass.getRenderpassHandle();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
    framebufferInfo.pAttachments = imageViews.data();
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(m_context->device.getLogicalDevice(), &framebufferInfo, nullptr, &m_framebuffer);
    if (result != VK_SUCCESS) Logger::fatal("Failed to create framebuffer");

    Logger::info("Successfully created framebuffer");
}

void VulkanFramebuffer::destroy() {
    vkDestroyFramebuffer(m_context->device.getLogicalDevice(), m_framebuffer, nullptr);
}