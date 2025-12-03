#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <set>

struct VulkanContext;

struct QueueFamilyIndices {
    // optional is wrapper that contains no value until something is assigned
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanDevice {
    public:
        void create(VulkanContext& context);
        void destroy();

        VkDevice& getLogicalDevice() { return m_logicalDevice; }
        SwapChainSupportDetails getSwapchainSupportDetails() { return m_swapChainSupport; }
        QueueFamilyIndices getQueueFamilyIndices() { return m_queueFamilyIndices; }
        VkFormat getDepthFormat() { return m_depth_format; }
        VkCommandPool& getCommandPool() { return m_graphicsCommandPool; }
        VkQueue& getGraphicsQueue() { return m_graphicsQueue; }
        VkQueue& getPresentQueue() { return m_presentQueue; }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    private:
        bool selectPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface);
        bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR& surface);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface);

        void createLogicalDevice(VkSurfaceKHR& surface);
        std::vector<const char*> getRequiredDeviceExtensions();
        void createGraphicsCommandPool();

        void findSupportedDepthFormat();
        
        VulkanContext* m_context;

        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

        QueueFamilyIndices m_queueFamilyIndices;
        VkDevice m_logicalDevice;
        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;

        SwapChainSupportDetails m_swapChainSupport;

        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        VkPhysicalDeviceMemoryProperties memory;

        VkCommandPool m_graphicsCommandPool = VK_NULL_HANDLE;

        VkFormat m_depth_format;

        std::vector<const char*> m_deviceExtensions = { 
            VK_KHR_SWAPCHAIN_EXTENSION_NAME // For presenting images to window 
        };
};