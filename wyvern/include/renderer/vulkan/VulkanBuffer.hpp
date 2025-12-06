#pragma once

#include <vulkan/vulkan.h>

struct VulkanContext;

class VulkanBuffer {
    public:
        void create(VulkanContext& context, VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_property_flags, bool bind_on_create = 1);
        void destroy();
        void bind(VkDeviceSize offset = 0);
        void loadData(const void* data, VkDeviceSize offset = 0);

        void copyBufferFrom(VulkanBuffer& src_buffer, VkQueue queue, VkDeviceSize size, VkDeviceSize src_offset = 0, VkDeviceSize dst_offset = 0);
        void copyBufferTo(VulkanBuffer& dst_buffer, VkQueue queue, VkDeviceSize size, VkDeviceSize src_offset = 0, VkDeviceSize dst_offset = 0);

        VkBuffer& getHandle() { return m_buffer; }
    
    private:
        void copyBufferToFrom(VkBuffer src_buffer, VkBuffer dst_buffer, VkQueue queue, VkDeviceSize size, VkDeviceSize src_offset = 0, VkDeviceSize dst_offset = 0);

        VulkanContext* m_context;

        VkBuffer m_buffer;
        VkDeviceMemory m_memory;
        VkDeviceSize m_size;

        bool is_bound;
};