#ifndef NO_VULKAN

#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

// Used for ShaderStage creation
struct ShaderStageCreateInfo {
    // ShaderStage name
    std::string name;
    // Path of the shader stage
    std::filesystem::path shaderPath;
    // Entry Point inside the shader file
    std::string entryPoint;
    // Used for buffer Bindings
    std::vector<const char*> bufferBindings;
};
/**
 * @brief A Shade node/Stage inside a pipelineStage
 */
class RAYX_API ShaderStage {
  public:
    ShaderStage(VkDevice& device, const ShaderStageCreateInfo& createInfo);
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

  private:
    VkDevice& m_Device;
    const char* m_name = nullptr;
    const char* m_entryPoint = nullptr;
    std::filesystem::path m_path;
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;

    void createShaderModule();
};
}  // namespace RAYX
#endif