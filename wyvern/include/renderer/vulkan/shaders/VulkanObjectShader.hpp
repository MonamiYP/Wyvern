#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct VulkanContext;

// A shader stage store info for one stage of the full graphics pipeline (e.g. vertex of fragment)
struct VulkanShaderStage {
    const char* type;
    VkShaderStageFlagBits flag;
    VkShaderModule shader_module;
    VkShaderModuleCreateInfo shader_module_create_info;
    VkPipelineShaderStageCreateInfo shader_stage_create_info;
};

// struct VulkanPipeline {
//     VkPipeline pipeline;
//     VkPipelineLayout pipeline_layout;
// };

class VulkanObjectShader {
    public:
        void create(VulkanContext& context, const std::string& base_path);
        void destroy();
        void use();

    private:
        VulkanContext* m_context;
        const uint32_t SHADER_STAGE_COUNT = 2;
        std::vector<VulkanShaderStage> m_vulkan_shader_stages;
        // VulkanPipeline m_pipeline;
};