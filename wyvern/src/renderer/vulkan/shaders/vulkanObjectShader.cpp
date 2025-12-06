#include "renderer/vulkan/shaders/VulkanObjectShader.hpp"
#include "renderer/vulkan/shaders/VulkanShaderUtils.hpp"
#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/Logger.hpp"

void VulkanObjectShader::create(VulkanContext& context, const std::string& base_path) {
    m_context = &context;

    m_vulkan_shader_stages.resize(SHADER_STAGE_COUNT);

    m_vulkan_shader_stages[0].type = "vert";
    m_vulkan_shader_stages[1].type = "frag";
    m_vulkan_shader_stages[0].flag = VK_SHADER_STAGE_VERTEX_BIT;
    m_vulkan_shader_stages[1].flag = VK_SHADER_STAGE_FRAGMENT_BIT;

    for (uint32_t i = 0; i < SHADER_STAGE_COUNT; i++) {
        VulkanShaderUtils::loadShaderStage(context, base_path + "." + std::string(m_vulkan_shader_stages[i].type) + ".spv", m_vulkan_shader_stages[i]);
    }

    /// Pipeline creation ///
    // Region of framebuffer that output will be rendered to
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) m_context->framebuffer_width;
    viewport.height = (float) m_context->framebuffer_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Scissor rectangles define in which regions pixels are actually stored
    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = m_context->swapchain.getSwapchainExtent();

    // Attributes
    uint32_t offset = 0;
    const uint32_t attribute_count = 1;
    std::vector<VkVertexInputAttributeDescription> attributes;
    attributes.resize(attribute_count);
    VkFormat formats[attribute_count] = { VK_FORMAT_R32G32B32_SFLOAT };
    uint64_t sizes[attribute_count] = { sizeof(glm::vec3) };
    for (uint32_t i = 0; i < attribute_count; ++i) {
        attributes[i].binding = 0;
        attributes[i].location = i;
        attributes[i].format = formats[i];
        attributes[i].offset = offset;
        offset += sizes[i];
    }

    //Stages
    std::vector<VkPipelineShaderStageCreateInfo> stage_create_info;
    stage_create_info.resize(SHADER_STAGE_COUNT);
    for (uint32_t i = 0; i < SHADER_STAGE_COUNT; ++i) {
        stage_create_info[i].sType = m_vulkan_shader_stages[i].shader_stage_create_info.sType;
        stage_create_info[i] = m_vulkan_shader_stages[i].shader_stage_create_info;
    }

    m_context->pipeline.create(context, attributes, {}, stage_create_info, viewport, scissor, false);
    Logger::info("Successfully created shader");
}

void VulkanObjectShader::destroy() {
    for (uint32_t i = 0; i < SHADER_STAGE_COUNT; ++i) {
        vkDestroyShaderModule(m_context->device.getLogicalDevice(), m_vulkan_shader_stages[i].shader_module, nullptr);
    }
    m_vulkan_shader_stages.clear();
}

void VulkanObjectShader::use() {
    m_context->pipeline.bind(m_context->commandBuffers[m_context->image_index], VK_PIPELINE_BIND_POINT_GRAPHICS);
}