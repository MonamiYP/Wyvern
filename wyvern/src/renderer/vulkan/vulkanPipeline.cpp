#include "renderer/vulkan/VulkanPipeline.hpp"
#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/Logger.hpp"
#include "core/Vertex.hpp"

void VulkanPipeline::create(VulkanContext& context, const std::vector<VkVertexInputAttributeDescription>& attributes, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const std::vector<VkPipelineShaderStageCreateInfo>& stages, VkViewport viewport, VkRect2D scissor, bool is_wireframe) {
    m_context = &context;
    // Viewport state
    VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rasterizer.depthClampEnable = VK_FALSE; 
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = is_wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // Multisamplign is one way to reduce anti aliasing, basically combines fragment shader results of multiple polygons that rasterize to the same pixel
    VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Depth and stencil testing
    VkPipelineDepthStencilStateCreateInfo depth_stencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable = VK_FALSE;

    /*
        Color from fragment shader combined with that already in framebuffer
        - VkPipelineColorBlendAttachmentState: color blending configuration per attached framebuffer
        - VkPipelineColorBlendStateCreateInfo: global color blending settings
    */
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo color_blending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;

    /*
        Most of the pipeline state needs to be baked into pipeline state (can't change)
        But a limited amount of states can be changed: e.g. viewport, linewidth, blend constants
        The following causes configuration of these states to be ignored, and so have to be specified at draw time
    */
    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH };
    VkPipelineDynamicStateCreateInfo dynamic_state = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    // Describe format of vertex data that will be passed onto vertex shader
    VkVertexInputBindingDescription vertex_input_binding;
    vertex_input_binding.binding = 0;
    vertex_input_binding.stride = sizeof(Vertex3D);
    vertex_input_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Move to next data entry for each vertex
    VkPipelineVertexInputStateCreateInfo vertex_input_info ={VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_input_binding; // Optional
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
    vertex_input_info.pVertexAttributeDescriptions = attributes.data(); // Optional

    // Describe geometry that will be drawn from the vertices
    VkPipelineInputAssemblyStateCreateInfo input_assembly={VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    /*
        Uniform values need to be specified during pipeline creation
    */
    VkPipelineLayoutCreateInfo pipeline_layout = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipeline_layout.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    pipeline_layout.pSetLayouts = descriptor_set_layouts.data();
    
    VkResult result = vkCreatePipelineLayout(m_context->device.getLogicalDevice(), &pipeline_layout, nullptr, &m_pipeline_layout);
    if (result != VK_SUCCESS) Logger::error("Failed to create pipeline layout");

    /*
        Finally, we can combine everything to create the pipeline
    */
    VkGraphicsPipelineCreateInfo pipeline_create_info { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipeline_create_info.stageCount = static_cast<uint32_t>(stages.size());;
    pipeline_create_info.pStages = stages.data();
    pipeline_create_info.pVertexInputState = &vertex_input_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly;
    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pRasterizationState = &rasterizer;
    pipeline_create_info.pMultisampleState = &multisampling;
    pipeline_create_info.pDepthStencilState = &depth_stencil;
    pipeline_create_info.pColorBlendState = &color_blending;
    pipeline_create_info.pDynamicState = &dynamic_state;
    pipeline_create_info.layout = m_pipeline_layout;
    pipeline_create_info.renderPass = m_context->renderpass.getHandle();
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;

    result = vkCreateGraphicsPipelines(m_context->device.getLogicalDevice(), VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_graphics_pipeline);
    if (result != VK_SUCCESS) Logger::error("vkCreateGraphicsPipelines failed with %s", result);
    else Logger::debug("Successfully created graphics pipeline");
}

void VulkanPipeline::destroy() {
    vkDestroyPipeline(m_context->device.getLogicalDevice(), m_graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(m_context->device.getLogicalDevice(), m_pipeline_layout, nullptr);
}

void VulkanPipeline::bind(VulkanCommandBuffer& command_buffer, VkPipelineBindPoint bind_point) {
    vkCmdBindPipeline(command_buffer.getHandle(), bind_point, m_graphics_pipeline);
}