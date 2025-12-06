#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanCommandBuffer.hpp"

struct VulkanContext;

class VulkanPipeline {
    public:
        void create(VulkanContext& context, const std::vector<VkVertexInputAttributeDescription>& attributes, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const std::vector<VkPipelineShaderStageCreateInfo>& stages, VkViewport viewport, VkRect2D scissor, bool is_wireframe);
        void destroy();
        void bind(VulkanCommandBuffer& command_buffer, VkPipelineBindPoint bind_point);

    private:
        VulkanContext* m_context;

        VkPipeline m_graphics_pipeline;
        VkPipelineLayout m_pipeline_layout;
};