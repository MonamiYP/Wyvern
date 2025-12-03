#pragma once

#include <vulkan/vulkan.h>
#include <vector>

/*
    A framebuffer is basically a collection of attachments
*/

struct VulkanContext;

class VulkanFramebuffer {
    public:
        void create(VulkanContext& context, uint32_t width, uint32_t height, const std::vector<VkImageView>& imageViews);
        void destroy();
        VkFramebuffer& getHandle() { return m_framebuffer; }

    private:
        VulkanContext* m_context;

        VkFramebuffer m_framebuffer;
};