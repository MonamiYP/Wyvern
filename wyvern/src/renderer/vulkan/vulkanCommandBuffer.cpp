#include "renderer/vulkan/VulkanCommandBuffer.hpp"

#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/Logger.hpp"

void VulkanCommandBuffer::allocate(VulkanContext& context, VkCommandPool& commandPool, bool is_primary) {
    m_context = &context;
    m_commandPool = &commandPool;

    VkCommandBufferAllocateInfo allocate_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocate_info.commandPool = commandPool;
    allocate_info.level = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocate_info.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(m_context->device.getLogicalDevice(), &allocate_info, &m_commandBuffer);
    if (result != VK_SUCCESS) Logger::fatal("Failed to allocate command buffer");

    m_state = CommandBufferState::READY;
}

void VulkanCommandBuffer::free() {
    vkFreeCommandBuffers(m_context->device.getLogicalDevice(), *m_commandPool, 1, &m_commandBuffer);
    m_commandBuffer = VK_NULL_HANDLE;
    m_state = CommandBufferState::NOT_ALLOCATED;
}

void VulkanCommandBuffer::beginRecording(bool is_single_use, bool is_renderpass_continue, bool is_simultaneous_use) {
    VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = 0;
    if (is_single_use) begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Command buffer is meant to only be used once and then be discarded
    if (is_renderpass_continue) begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Command buffer is a secondary buffer that will be used inside renderpass cbegun by a primary buffer
    if (is_simultaneous_use) begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Command buffer intended to be resubmitted before previous submition is compelte

    VkResult result = vkBeginCommandBuffer(m_commandBuffer, &begin_info);
    if (result != VK_SUCCESS) Logger::fatal("Failed to begin recording command buffer!");

    m_state = CommandBufferState::RECORDING;
}

void VulkanCommandBuffer::endRecording() {
    VkResult result = vkEndCommandBuffer(m_commandBuffer);
    if (result != VK_SUCCESS) Logger::fatal("Failed to record command buffer");
    m_state = CommandBufferState::RECORDING_ENDED;
}

void VulkanCommandBuffer::updateSubmitted() {
    m_state = CommandBufferState::SUBMITTED;
}

void VulkanCommandBuffer::reset() {
    /** 
        We have previosuly set VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT when creating the command pool so that command buffers can be individually reset without having to reset the entire command pool
        But we don't need to call vkResetCommandBuffer explicitly because vkBeginCommandBuffer will reset to the initial state implicitly
        Just need to make sure that the GPU is not currently executing the command buffer (using fences to wait for GPU to finish)
    */
    m_state = CommandBufferState::READY;
}

void VulkanCommandBuffer::allocateAndBeginSingleUse(VulkanContext& context, VkCommandPool& commandPool) {
    allocate(context, commandPool);
    beginRecording(1, 0, 0);
}

void VulkanCommandBuffer::endSingleUse(VkQueue queue) {
    endRecording();

    VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_commandBuffer;

    VkResult result = vkQueueSubmit(queue, 1, &submit_info, 0);
    if (result != VK_SUCCESS) Logger::fatal("Failed to submit single use command buffer");

    vkQueueWaitIdle(queue); // Wait for queue to finish executing commands in command buffer, could add fence in the future if needed
    free();
}