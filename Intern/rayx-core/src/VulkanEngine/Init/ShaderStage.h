#ifndef NO_VULKAN

#pragma once

#include <vulkan/vulkan.hpp>

#include "RAY-Core.h"

namespace RAYX {

// Used for ShaderStage creation
struct ShaderStageCreateInfo {
    std::string name;                  // ShaderStage name
    std::filesystem::path shaderPath;  // Path of the shader stage
    std::string entryPoint;            // Entry Point inside the shader file
    VkShaderStageFlagBits shaderType = VK_SHADER_STAGE_COMPUTE_BIT;
};
/**
 * @brief A ShaderStage (Node) inside a pipelineStage
 */
class RAYX_API ShaderStage {
  public:
    ShaderStage(VkDevice& device, const ShaderStageCreateInfo& createInfo);
    ~ShaderStage();
    ShaderStage(const ShaderStage& other) = default;

    [[nodiscard]] const VkShaderModule& getShaderModule() const { return m_shaderModule; }
    // Used for Pipeline creation
    VkPipelineShaderStageCreateInfo getPipelineShaderCreateInfo();
    [[nodiscard]] VkShaderStageFlagBits getShaderStageFlagBits() const { return m_shaderType; }

  private:
    VkDevice& m_Device;
    std::string m_name;
    std::string m_entryPoint;
    std::filesystem::path m_path;
    VkShaderStageFlagBits m_shaderType;
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;

    void createShaderModule();
};
}  // namespace RAYX
#endif