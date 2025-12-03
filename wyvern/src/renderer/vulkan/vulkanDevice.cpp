#include "renderer/vulkan/VulkanDevice.hpp"

#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/Logger.hpp"

void VulkanDevice::create(VulkanContext& context) {
    m_context = &context;
    if (selectPhysicalDevice(m_context->instance, m_context->surface)) Logger::info("Successfully selected physical device");
    createLogicalDevice(m_context->surface);
    createGraphicsCommandPool();
    findSupportedDepthFormat();
}

void VulkanDevice::destroy() {
    vkDestroyCommandPool(m_context->device.getLogicalDevice(), m_graphicsCommandPool, nullptr);
    vkDestroyDevice(m_logicalDevice, nullptr);
}

bool VulkanDevice::selectPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface) {
    /*
        Look for and select graphics card that supports the features needed
    */
    uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
    if (physical_device_count == 0) {
        Logger::fatal("Failed to find GPUs with Vulkan support");
        return false;
    }
    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data());

    for (const auto& physical_device : physical_devices) {
        if (isDeviceSuitable(physical_device, surface)) {
            m_physicalDevice = physical_device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        Logger::fatal("Failed to find a suitable GPU");
        return false;
    }

    return true;
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR& surface) {
    /*
        Check if device is up to specifications we want
    */
    QueueFamilyIndices indices = findQueueFamilies(device, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        m_swapChainSupport = querySwapChainSupport(device, surface);
        swapChainAdequate = !m_swapChainSupport.formats.empty() && !m_swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    /*
        Enumerate the extensions and check if all of the required extensions are amongst them
    */
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const auto& extension : availableExtensions) 
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface) {
    /*
        Queue families are hardware queues that handle different operations, e.g. graphics or compute shaders
        Each family of queues allows only a subset of commands, everytime you want to do any sort of GPU based rendering, commands need to be taken from command buffer and submit them to queue
        findQueueFamilies() looks for all queue families required
    */

    // Retrieve list of queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) m_queueFamilyIndices.graphicsFamily = i; // Check for graphics support

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) m_queueFamilyIndices.presentFamily = i;

        if (m_queueFamilyIndices.isComplete()) break;
        i++;
    }

    return m_queueFamilyIndices;
}

SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface) {
    /*
        Swapchain may not be compatible with window surface
        Need to query for some more details 
    */
    SwapChainSupportDetails details;

    // Surface capabilities are stuff like min/max number of images in swap chain, min/max width and height of images
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    // Surface formats are stuff like pixel format and color space
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    // Presentation modes defines how images are presented to the screen, e.g. vsync
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void VulkanDevice::createLogicalDevice(VkSurfaceKHR& surface) {
    /*
        A logical device interfaces with the phsycial device
    */

    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice, surface);

    // Describes number of queues we want for a single queue family
    std::vector<VkDeviceQueueCreateInfo> create_infos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    // Priorities influence scheduling of command buffer execution
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo create_info = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        create_info.queueFamilyIndex = queueFamily;
        create_info.queueCount = 1;
        create_info.pQueuePriorities = &queuePriority;
        create_infos.push_back(create_info);
    }

    // Specify the set of device features to be used (none for now)
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    create_info.pEnabledFeatures = &deviceFeatures;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(create_infos.size());
    create_info.pQueueCreateInfos = create_infos.data();
    create_info.enabledLayerCount = 0;

    std::vector<const char *> requiredExtensions = getRequiredDeviceExtensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    create_info.ppEnabledExtensionNames = requiredExtensions.data();

    if (vkCreateDevice(m_physicalDevice, &create_info, nullptr, &m_logicalDevice) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device");

    vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue);
}

std::vector<const char*> VulkanDevice::getRequiredDeviceExtensions() {
    /*
        Returns list of required extensions (for device)
    */
    uint32_t deviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> deviceExtensionProperties(deviceExtensionCount);
    vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionCount, deviceExtensionProperties.data());

    for (const auto& extension : deviceExtensionProperties)
        if (std::string(extension.extensionName) == "VK_KHR_portability_subset") {
            m_deviceExtensions.push_back("VK_KHR_portability_subset"); // MoltenVK compatibility (macOS)
            break;
        }
    
    return m_deviceExtensions;
}

void VulkanDevice::findSupportedDepthFormat() {
    const std::vector<VkFormat> candidates = { // Ordered from most desireable to least desirable
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    VkFormatFeatureFlags flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

        if ((props.linearTilingFeatures & flags) == flags || (props.optimalTilingFeatures & flags) == flags) { m_depth_format = format;
            return;
        }
    }

    Logger::fatal("No suitable depth format found");
}

void VulkanDevice::createGraphicsCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice, m_context->surface);

    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VkResult result = vkCreateCommandPool(m_context->device.getLogicalDevice(), &createInfo, nullptr, &m_graphicsCommandPool);
    if (result != VK_SUCCESS) Logger::fatal("Failed to create command pool!");

    Logger::info("Created graphics command pool");
}

uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    Logger::fatal("Failed to find suitable memory type!");
    return 0;
}