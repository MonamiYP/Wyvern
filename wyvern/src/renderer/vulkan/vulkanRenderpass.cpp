#include "renderer/vulkan/VulkanRenderpass.hpp"

#include "renderer/vulkan/VulkanBackend.hpp"

#include "core/Logger.hpp"

void VulkanRenderpass::create(VulkanContext& context, glm::vec2 size, glm::vec2 offset, glm::vec4 color, float depth, float stencil) {
    m_context = &context;
    m_size = size;
    m_offset = offset;
    m_color = color;
    m_depth = depth;
    m_stencil = stencil;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    int attachment_description_count = 2; // Attachments are images the renderpass will render into. This will be configurable in the future
    VkAttachmentDescription attachment_descriptions[attachment_description_count];

    VkAttachmentDescription color_attachment{};
    color_attachment.format = m_context->swapchain.getImageFormat();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear framebuffer to black before drawing new frame
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // rendered contents stored in memory and can be read later
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // images to be presented in the swap chain
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = m_context->device.getDepthFormat();
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachment_descriptions[0] = color_attachment;
    attachment_descriptions[1] = depth_attachment;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    // TODO: may implement the other attachments. Could move to separate functions.

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

    VkRenderPassCreateInfo renderpass_create_info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    renderpass_create_info.attachmentCount = attachment_description_count;
    renderpass_create_info.pAttachments = attachment_descriptions;
    renderpass_create_info.subpassCount = 1;
    renderpass_create_info.pSubpasses = &subpass;
    renderpass_create_info.dependencyCount = 1;
    renderpass_create_info.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(m_context->device.getLogicalDevice(), &renderpass_create_info, nullptr, &m_renderPass);
    if (result != VK_SUCCESS) Logger::fatal("Failed to create render pass");

    Logger::info("Successfully created renderpass");
}

void VulkanRenderpass::destroy() {
    vkDestroyRenderPass(m_context->device.getLogicalDevice(), m_renderPass, nullptr);
}

void VulkanRenderpass::begin(VulkanCommandBuffer* commandBuffer, VkFramebuffer& framebuffer) {
    VkRenderPassBeginInfo renderpass_begin_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    renderpass_begin_info.renderPass = m_renderPass;
    renderpass_begin_info.framebuffer = framebuffer;
    renderpass_begin_info.renderArea.offset = { static_cast<int>(m_offset.x), static_cast<int>(m_offset.y) }; 
    renderpass_begin_info.renderArea.extent = { static_cast<unsigned int>(m_size.x), static_cast<unsigned int>(m_size.y) };
    VkClearValue clear_values[2];
    clear_values[0].color = {{ m_color.r, m_color.g, m_color.b, m_color.a }};
    clear_values[1].depthStencil.depth = m_depth;
    clear_values[1].depthStencil.stencil = m_stencil;
    renderpass_begin_info.clearValueCount = 2;
    renderpass_begin_info.pClearValues = clear_values;

    vkCmdBeginRenderPass(commandBuffer->getHandle(), &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    commandBuffer->setState(CommandBufferState::IN_RENDER_PASS);
}

void VulkanRenderpass::end(VulkanCommandBuffer* commandBuffer) {
    vkCmdEndRenderPass(commandBuffer->getHandle());
    commandBuffer->setState(CommandBufferState::RECORDING);
}