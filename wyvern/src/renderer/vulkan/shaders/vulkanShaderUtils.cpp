#include "renderer/vulkan/shaders/VulkanShaderUtils.hpp"
#include "renderer/vulkan/VulkanBackend.hpp"
#include "core/Logger.hpp"

void VulkanShaderUtils::loadShaderStage(VulkanContext& context, const std::string& path, VulkanShaderStage& shader_stage) {
    if (createShaderModule(context, path, shader_stage)) {
        shader_stage.shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage.shader_stage_create_info.stage = shader_stage.flag;
        shader_stage.shader_stage_create_info.module = shader_stage.shader_module;
        shader_stage.shader_stage_create_info.pName = "main";
    } else Logger::error("Failed to load shader stage for: %s", path.c_str());
}

bool VulkanShaderUtils::createShaderModule(VulkanContext& context, const std::string& path, VulkanShaderStage& shader_stage){
    std::vector<char> code = readFile(path);

    shader_stage.shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_stage.shader_module_create_info.codeSize = code.size();
    shader_stage.shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkResult result = vkCreateShaderModule(context.device.getLogicalDevice(), &shader_stage.shader_module_create_info, nullptr, &shader_stage.shader_module);
    if (result != VK_SUCCESS) return false;
    return true;
}

std::vector<char> VulkanShaderUtils::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) Logger::error("Failed to open shader file: %s", path.c_str());

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}