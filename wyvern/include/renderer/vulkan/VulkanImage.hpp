#pragma once

#include <vulkan/vulkan.h>

/*
    A VkImage is basically GPU memory allocated for storing pixel data (like color, depth, etc.)
    A VkImageView is literally a view into the image, which describes how to access image and which part of the image to access (e.g. should it be treated as a 2D texture depth without any mipmapping levels?)
*/

struct VulkanContext;

class VulkanImage {
    public:
        void create(VulkanContext& context, VkImageType imageType, int width, int height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_flags, bool create_view, VkImageAspectFlags view_aspect_flags);
        void createImageView(VkFormat format, VkImageAspectFlags view_aspect_flags);
        void destroy();

        VkImageView& getImageView() { return m_imageView; }

    private:
        VulkanContext* m_context;
        VkImage m_image;
        VkImageView m_imageView;
        VkDeviceMemory m_deviceMemory;

        int m_width;
        int m_height;
};