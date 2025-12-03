#pragma once

#include <vulkan/vulkan.h>

/*
    A bit about Vulkan synchronisation:
    All Vulkan commands that are executed go into a queue and are executed 'non-stop' at an undefined order
    Within a queue the commands are executed lienarly, but say you have many queues, then the order of operations is not guaranteed
    Therefore, we want to be able to allow CPU to sync execution of commands with the GPU, and control the order of operations in the GPU
    FENCES:
    - Used for GPU->CPU communication
    - If a fence is set, we can know from the CPU if the GPU has finished its queue operations --> via vkWaitForFences

    SEMAPHORES:
    - Used for GPU to GPU sync
    - Allows defining order of operation on GPU commands --> some Vulkan operations allow for either a signal or wait semaphore:
        - If set to signal a semaphore, the operation will lock the semaphore when it executes and unlock once finished
        - If set to wait a semaphore, operation will wait to begin execution until semaphore is unlocked
*/

struct VulkanContext;

class VulkanFence {
    public:
        void create(VulkanContext& context, bool create_signaled = 0);
        void destroy();

        bool wait(uint64_t nanosec);
        void reset();

        VkFence& getHandle() { return m_fence; }

    private:
        VulkanContext* m_context;
        VkFence m_fence;
        bool is_signaled;
};