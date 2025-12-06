#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <fstream>
#include "renderer/vulkan/shaders/VulkanObjectShader.hpp"

class VulkanShaderUtils {
    public:
        static void loadShaderStage(VulkanContext& context, const std::string& path, VulkanShaderStage& shader_stage);
    private:
        static bool createShaderModule(VulkanContext& context, const std::string& path, VulkanShaderStage& shader_stages);
        static std::vector<char> readFile(const std::string& path);
};