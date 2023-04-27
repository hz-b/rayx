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
    std::vector<const char*> buffers;
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
          m_shaderModule(std::move(other.m_shaderModule)),
          m_DescriptorBindings(std::move(other.m_DescriptorBindings)) {}

    const VkShaderModule& getShaderModule() const { return m_shaderModule; }

    // Used for Pipeline creation
    VkPipelineShaderStageCreateInfo getPipelineShaderCreateInfo();
    // Get bindings associated with the current Shader Module
    std::vector<VkDescriptorSetLayoutBinding> getDescriptorBindings();
    // Add/replace a binding
    // The buffer has to be an existent buffer!
    void addBufferBinding(uint32_t binding, const char* buffer);

  private:
    VkDevice& m_Device;
    const char* m_name = nullptr;
    const char* m_entryPoint = nullptr;
    std::filesystem::path m_path;
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    std::map<uint32_t, const char*> m_DescriptorBindings;

    void createShaderModule();
};
}  // namespace RAYX
#endif