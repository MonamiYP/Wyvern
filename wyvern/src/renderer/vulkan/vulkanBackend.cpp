#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/glfw/Window.hpp"
#include "core/Logger.hpp"
#include "renderer/vulkan/VulkanBackend.hpp"

/*
    Vulkan setup functions
*/

void VulkanBackend::init(const char* appName, Window* window) {
    m_context.window = window;
    int width, height;

    createInstance(appName);
    createDebugCallback();

    window->createVulkanSurface(m_context.instance, m_context.surface);
    window->getFramebufferSize(width, height);

    m_context.framebuffer_width = width;
    m_context.framebuffer_height = height;

    m_context.device.create(m_context);
    m_context.swapchain.create(width, height, m_context);
    m_context.renderpass.create(m_context, glm::vec2(width, height), glm::vec2(0, 0), glm::vec4(0.6f, 0.2f, 0.05f, 1.0f), 1.0f, 0);
    m_context.swapchain.regenerateFramebuffers();

    createCommandBuffers();
    createSyncObjects();
}

void VulkanBackend::createInstance(const char* appName) {
    /*
        Setup vulkan instance
        An instance is the conection between your application and the Vulkan library
        Initialise Vulkan library by creating an instance
    */

    // Optional info about the app
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.pNext = NULL;
    app_info.pApplicationName = appName;
    app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.pEngineName = "Wyvern Engine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    // Required info about instance, tells Vulkan driver which global extensions and validation layers we want to use. 
    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pNext = NULL;
    create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // This is only needed for mac
    create_info.pApplicationInfo = &app_info;

    auto extensions = getRequiredInstanceExtensions();   
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    auto layers = getLayers();
    create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    create_info.ppEnabledLayerNames = layers.data();

    VkResult result = vkCreateInstance(&create_info, nullptr, &m_context.instance);
    if (result != VK_SUCCESS) {
        Logger::error("vkCreateInstance failed with code: %d", result);
    }
}

std::vector<const char*> VulkanBackend::getRequiredInstanceExtensions() {
    /*
        Returns list of required extensions (for instance) based on some conditions
        - Extensions specified by GLFW (always required)
        - Debug utils added is validaion layers are enabled
    */
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = m_context.window->getGLFWExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions;
    for(uint32_t i = 0; i < glfwExtensionCount; i++)
        extensions.emplace_back(glfwExtensions[i]);

    // This is only for mac, TODO: generalise for other platforms
    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    # if defined(_DEBUG)
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        Logger::info("Required extensions:");
        for (auto ext : extensions) {
            Logger::info("      %s", ext);
        }
    # endif

    return extensions;
}

std::vector<const char*> VulkanBackend::getLayers() {
    # if defined(_DEBUG)
        m_context.layers.emplace_back("VK_LAYER_KHRONOS_validation");
        Logger::info("Validation layer enabled.");
    # endif

    return m_context.layers;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    switch (messageSeverity) {
        default:
            Logger::error("Default debug callback");
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            Logger::info("%s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Logger::info("%s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Logger::warn("%s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Logger::error("%s", pCallbackData->pMessage);
            break;
    }

    return VK_FALSE;
}

void VulkanBackend::createDebugCallback() {
    #if defined(_DEBUG)
        VkDebugUtilsMessengerCreateInfoEXT create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = &debugCallback;
        create_info.pUserData = nullptr;

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_context.instance, "vkCreateDebugUtilsMessengerEXT");

        if (func != nullptr) {
            func(m_context.instance, &create_info, NULL, &m_context.debugMessenger);
            Logger::info("Vulkan debugger created");
        }
        else
            Logger::error("Cannot find vkCreateDebugUtilsMessenger");
    # endif
}

void VulkanBackend::createCommandBuffers() {
    m_context.commandBuffers.resize(m_context.swapchain.getImageCount());
    for (int i = 0; i < m_context.commandBuffers.size(); i++) {
        if (m_context.commandBuffers[i].getHandle() != VK_NULL_HANDLE) m_context.commandBuffers[i].free();
        m_context.commandBuffers[i].allocate(m_context, m_context.device.getCommandPool(), true);
    }
}

void VulkanBackend::createSyncObjects() {
    int frame_count = m_context.max_frames_in_flight;
    m_context.image_acquire_semaphores.resize(frame_count);
    m_context.queue_submit_semaphores.resize(m_context.swapchain.getImageCount());
    m_context.in_flight_fences.resize(frame_count);
    m_context.images_in_flight.resize(m_context.swapchain.getImageCount(), nullptr);

    for (int i = 0; i < frame_count; i++) {
        VkSemaphoreCreateInfo semaphore_create_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(m_context.device.getLogicalDevice(), &semaphore_create_info, nullptr, &m_context.image_acquire_semaphores[i]);

        m_context.in_flight_fences[i].create(m_context, true);
    }

    for (int i = 0; i < m_context.swapchain.getImageCount(); i++) {
        VkSemaphoreCreateInfo semaphore_create_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(m_context.device.getLogicalDevice(), &semaphore_create_info, nullptr, &m_context.queue_submit_semaphores[i]);
    }
}

void VulkanBackend::recreateSwapchain() {
    vkDeviceWaitIdle(m_context.device.getLogicalDevice());
    m_context.swapchain.recreate();
    m_context.renderpass.setNewSize(m_context.framebuffer_width, m_context.framebuffer_height);
    m_context.swapchain.regenerateFramebuffers();

    createCommandBuffers();
    cleanupSyncObjects();
    createSyncObjects();
}

/*
    Vulkan cleanup functions
*/
void VulkanBackend::shutdown() {
    vkDeviceWaitIdle(m_context.device.getLogicalDevice());

    cleanupSyncObjects();
    
    for (auto& command_buffer : m_context.commandBuffers) command_buffer.free();

    m_context.renderpass.destroy();
    m_context.swapchain.destroy();
    m_context.device.destroy();

    # if defined(_DEBUG)
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(m_context.instance, m_context.debugMessenger, NULL);
    # endif
    
    vkDestroySurfaceKHR(m_context.instance, m_context.surface, nullptr);
    vkDestroyInstance(m_context.instance, nullptr);

    Logger::info("Destroyed all Vulkan objects");
}

void VulkanBackend::cleanupSyncObjects() {
    for (auto& semaphore : m_context.image_acquire_semaphores) vkDestroySemaphore(m_context.device.getLogicalDevice(), semaphore, nullptr);
    for (auto& semaphore : m_context.queue_submit_semaphores) vkDestroySemaphore(m_context.device.getLogicalDevice(), semaphore, nullptr);
    for (auto& fence : m_context.in_flight_fences) fence.destroy();

    m_context.image_acquire_semaphores.clear();
    m_context.queue_submit_semaphores.clear();
    m_context.in_flight_fences.clear();
}

/*
    Vulkan drawing functions
*/

bool VulkanBackend::beginFrame(float dt) {
    int current_frame = m_context.current_frame;

    // Once fence is free it will be allowed to move on, if fence taking long time return
    if (!m_context.in_flight_fences[current_frame].wait(UINT64_MAX)) return false;
    
    VkResult result = m_context.swapchain.acquireNextImageIndex(m_context.image_acquire_semaphores[current_frame], &m_context.image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_context.window_resized) {
        recreateSwapchain();
        m_context.window_resized = false;
        return false;
    } else if (result != VK_SUCCESS) {
        Logger::fatal("Failed to acquire swapchain image!");
        return false;
    }

    // Begin recording commands and then renderpass for current frame
    VulkanCommandBuffer* command_buffer = &m_context.commandBuffers[m_context.image_index];
    command_buffer->reset();
    command_buffer->beginRecording();
    m_context.renderpass.begin(command_buffer, m_context.swapchain.getFrameBuffer(m_context.image_index).getHandle());

    return true;
}

void VulkanBackend::endFrame(float dt) {
    VulkanCommandBuffer* command_buffer = &m_context.commandBuffers[m_context.image_index];

    m_context.renderpass.end(command_buffer);
    command_buffer->endRecording();

    // Make sure previous frame was not using this image, if it was then wait for it to complete
    if (m_context.images_in_flight[m_context.image_index] != VK_NULL_HANDLE) m_context.images_in_flight[m_context.image_index]->wait(UINT64_MAX);

    m_context.images_in_flight[m_context.image_index] = &m_context.in_flight_fences[m_context.current_frame];
    m_context.in_flight_fences[m_context.current_frame].reset();

    VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    VkPipelineStageFlags flags[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.pWaitDstStageMask = flags;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->getHandle();
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &m_context.queue_submit_semaphores[m_context.image_index];
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &m_context.image_acquire_semaphores[m_context.current_frame];

    VkResult result = vkQueueSubmit(m_context.device.getGraphicsQueue(), 1, &submit_info, m_context.in_flight_fences[m_context.current_frame].getHandle());
    if (result != VK_SUCCESS) Logger::error("vkQueueSubmit failed with result: %s", result);
    command_buffer->updateSubmitted();

    m_context.swapchain.present(m_context.device.getPresentQueue(), m_context.queue_submit_semaphores[m_context.image_index], m_context.image_index);
    
    m_context.current_frame = (m_context.current_frame + 1) % m_context.max_frames_in_flight;
}

void VulkanBackend::onWindowResize(int width, int height) {
    m_context.framebuffer_width = width;
    m_context.framebuffer_height = height;
    m_context.window_resized = true;
}