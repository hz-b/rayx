#ifndef NO_VULKAN

#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"

namespace RAYX {

// Used for ShaderStage creation
struct ShaderStageCreateInfo_t {
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
    ShaderStage(VkDevice& device, const ShaderStageCreateInfo_t& createInfo);
    ~ShaderStage();
    ShaderStage(const ShaderStage& other)
        : m_Device(other.m_Device),
          m_name(std::move(other.m_name)),
          m_entryPoint(std::move(other.m_entryPoint)),
          m_path(std::move(other.m_path)),
          m_shaderModule(std::move(other.m_shaderModule)) {}

    const VkShaderModule& getShaderModule() const { return m_shaderModule; }
    // Used for Pipeline creation
    VkPipelineShaderStageCreateInfo getPipelineShaderCreateInfo();
    const VkShaderStageFlagBits getShaderStageFlagBits() const { return m_shaderType; }

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