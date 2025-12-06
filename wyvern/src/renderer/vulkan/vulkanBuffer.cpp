#include "renderer/vulkan/VulkanBuffer.hpp"
#include "renderer/vulkan/VulkanBackend.hpp"
#include "renderer/vulkan/VulkanCommandBuffer.hpp"
#include "core/Logger.hpp"

void VulkanBuffer::create(VulkanContext& context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool bind_on_create) {
    m_context = &context;
    m_size = size;

    VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    buffer_info.size = m_size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(m_context->device.getLogicalDevice(), &buffer_info, nullptr, &m_buffer);
    if (result != VK_SUCCESS) {
        Logger::error("Failed to create Vulkan buffer");
        return;
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(m_context->device.getLogicalDevice(), m_buffer, &mem_requirements);

    VkMemoryAllocateInfo allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocate_info.allocationSize = mem_requirements.size;
    allocate_info.memoryTypeIndex = m_context->device.findMemoryType(mem_requirements.memoryTypeBits, properties);

    result = vkAllocateMemory(m_context->device.getLogicalDevice(), &allocate_info, nullptr, &m_memory);
    if (result != VK_SUCCESS) {
        Logger::error("Unable to create vulkan buffer because the required memory allocation failed. Error: %i", result);
        return;
    }

    if (bind_on_create) bind();
}

void VulkanBuffer::destroy() {
    vkDeviceWaitIdle(m_context->device.getLogicalDevice());
    is_bound = false;
    vkDestroyBuffer(m_context->device.getLogicalDevice(), m_buffer, nullptr);
    vkFreeMemory(m_context->device.getLogicalDevice(), m_memory, nullptr);
}

void VulkanBuffer::bind(VkDeviceSize offset) {
    VkResult result = vkBindBufferMemory(m_context->device.getLogicalDevice(), m_buffer, m_memory, offset);
    if (result != VK_SUCCESS) Logger::error("Failed to bind buffer");
}

void VulkanBuffer::loadData(const void* data, VkDeviceSize offset) {
    void* data_ptr;
    VkResult result = vkMapMemory(m_context->device.getLogicalDevice(), m_memory, offset, m_size, 0, &data_ptr);
    memcpy(data_ptr, data, m_size);
    vkUnmapMemory(m_context->device.getLogicalDevice(), m_memory);
}

void VulkanBuffer::copyBufferFrom(VulkanBuffer& src_buffer, VkQueue queue, VkDeviceSize size, VkDeviceSize src_offset, VkDeviceSize dst_offset) {
    copyBufferToFrom(src_buffer.getHandle(), m_buffer, queue, size, src_offset, dst_offset);
}

void VulkanBuffer::copyBufferTo(VulkanBuffer& dst_buffer, VkQueue queue, VkDeviceSize size, VkDeviceSize src_offset, VkDeviceSize dst_offset) { 
    copyBufferToFrom(m_buffer, dst_buffer.getHandle(), queue, size, src_offset, dst_offset); 
};

void VulkanBuffer::copyBufferToFrom(VkBuffer src_buffer, VkBuffer dst_buffer, VkQueue queue, VkDeviceSize size, VkDeviceSize src_offset, VkDeviceSize dst_offset) {
    VulkanCommandBuffer temp_command_buffer;
    temp_command_buffer.allocateAndBeginSingleUse(*m_context, m_context->device.getCommandPool());

    VkBufferCopy copy_region {};
    copy_region.srcOffset = src_offset;
    copy_region.dstOffset = dst_offset;
    copy_region.size = size;
    vkCmdCopyBuffer(temp_command_buffer.getHandle(), src_buffer, dst_buffer, 1, &copy_region);
    temp_command_buffer.endSingleUse(queue);
}