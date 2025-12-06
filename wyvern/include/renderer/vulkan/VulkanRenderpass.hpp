#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "renderer/vulkan/VulkanCommandBuffer.hpp"

/*
    All rendering commands have to happen inside a renderpass
    A renderpass allows the driver to know more about the state of the images to be rendered. We must tell Vulkan how exactly we will use the framebuffer. So basically the renderpass knows:
    - Which attachments (color, depth, stencil) is to be used
    - How to load, store and clear these attachments
    - The subpasses (rendering stages that can use attachments efficiently)
    - Dependencies to ensure GPU commands happen in the right order

    (Recently, something called Dynamic Rendering has been introduced which means you don't need renderpasses and framebuffers and allows greater flexibility when setting up render target)
*/

struct VulkanContext;

enum class RenderpassState {
    READY,
    RECORDING,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
};

class VulkanRenderpass {
    public:
        void create(VulkanContext& context, glm::vec2 size, glm::vec2 offset, glm::vec4 color, float depth, float stencil);
        void destroy();

        void begin(VulkanCommandBuffer* commandBuffer, VkFramebuffer& framebuffer);
        void end(VulkanCommandBuffer* commandBuffer);

        VkRenderPass& getHandle() { return m_renderPass; }
        void setNewSize(int width, int height) { m_size = glm::vec2(width, height); }
    
    private:
        VulkanContext* m_context;

        VkRenderPass m_renderPass;
        RenderpassState m_state;

        glm::vec2 m_size; // width and height
        glm::vec2 m_offset; // x offset and y offset
        glm::vec4 m_color;
        float m_depth;
        float m_stencil;
};