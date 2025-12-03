#pragma once

#include <vulkan/vulkan.h>

/*
    COMMAND BUFFER:
    - Command buffers are a list of commands prepared on the CPU and sent to the GPU for execution.
    - Commands include things like drawing operations and memory transfers
    - These commands need to be stored in command buffer objects, such that all commands are submitted together and Vulkan can more efficiently process commands
    COMMAND POOL:
    - Created before a command buffer is created
    - Command buffers are frequently reset and resued, we do not want to recreate them so the command pool let us manage the memory that is used to store the buffers
*/

struct VulkanContext;

enum class CommandBufferState {
    READY, // Allocated, not recording
    RECORDING, // Currently being recorded
    IN_RENDER_PASS, // Recording a renderpass inside the command buffer
    RECORDING_ENDED, // Finished recording, ready to submit
    SUBMITTED, // Submitted to a queue
    NOT_ALLOCATED // Uninitialised or freed
};

class VulkanCommandBuffer {
    public:
        void allocate(VulkanContext& context, VkCommandPool& commandPool, bool is_primary = 1);
        void free();

        void beginRecording(bool is_single_use = 0, bool is_renderpass_continue = 0, bool is_simultaneous_use = 0);
        void endRecording();
    
        void allocateAndBeginSingleUse(VulkanContext& context, VkCommandPool& commandPool);
        void endSingleUse(VkQueue queue);

        void updateSubmitted();
        void reset();        

        void setState(CommandBufferState state) { m_state = state; }

        VkCommandBuffer& getHandle() { return m_commandBuffer; }

    private:
        VulkanContext* m_context;
        VkCommandPool* m_commandPool;
        
        VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
        CommandBufferState m_state = CommandBufferState::NOT_ALLOCATED;
};