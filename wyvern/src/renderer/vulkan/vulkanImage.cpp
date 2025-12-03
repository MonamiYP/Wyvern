#include "renderer/vulkan/VulkanImage.hpp"
#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/Logger.hpp"

void VulkanImage::create(VulkanContext& context, VkImageType imageType, int width, int height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_flags, bool create_view, VkImageAspectFlags view_aspect_flags) {

    m_context = &context;
    m_width = width;
    m_height = height;
    
    VkImageCreateInfo image_create_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.extent.depth = 1;  
    image_create_info.mipLevels = 4; 
    image_create_info.arrayLayers = 1;  
    image_create_info.format = format;
    image_create_info.tiling = tiling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = usage;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(context.device.getLogicalDevice(), &image_create_info, nullptr, &m_image);
    if (result != VK_SUCCESS) Logger::fatal("Failed to create image");

    // At this point we haven't allocated/bound any memory for the image yet, which we do below:

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context.device.getLogicalDevice(), m_image, &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context.device.findMemoryType(memory_requirements.memoryTypeBits, memory_flags);
    result = vkAllocateMemory(context.device.getLogicalDevice(), &memory_allocate_info, nullptr, &m_deviceMemory);
    if (result != VK_SUCCESS) Logger::fatal("Failed to allocate image memory");

    vkBindImageMemory(context.device.getLogicalDevice(), m_image, m_deviceMemory, 0);

    Logger::info("Successfully created image");

    // Create view
    if (create_view) {
        createImageView(format, view_aspect_flags);
    }
}

void VulkanImage::createImageView(VkFormat format, VkImageAspectFlags view_aspect_flags) {
    VkImageViewCreateInfo view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    view_create_info.image = m_image;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;  // TODO: Make configurable.
    view_create_info.format = format;
    view_create_info.subresourceRange.aspectMask = view_aspect_flags;

    // TODO: Make configurable
    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount = 1;

    VkResult result = vkCreateImageView(m_context->device.getLogicalDevice(), &view_create_info, nullptr, &m_imageView);
    if (result != VK_SUCCESS) Logger::fatal("Could not create image view");

    Logger::info("Successfully created image view");
}

void VulkanImage::destroy() {
    if (m_imageView != VK_NULL_HANDLE) vkDestroyImageView(m_context->device.getLogicalDevice(), m_imageView, nullptr);
    if (m_image != VK_NULL_HANDLE) vkDestroyImage(m_context->device.getLogicalDevice(), m_image, nullptr);
    if (m_deviceMemory != VK_NULL_HANDLE) vkFreeMemory(m_context->device.getLogicalDevice(), m_deviceMemory, nullptr);
}