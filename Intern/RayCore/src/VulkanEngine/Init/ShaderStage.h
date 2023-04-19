#ifndef NO_VULKAN

#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
/**
 * @brief A Shade node/Stage
 *
 */

struct ShaderStageCreateInfo {
    std::string name;
    std::filesystem::path shaderPath;
    std::string entryPoint;
};

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
          descriptorSetLayout(std::move(other.descriptorSetLayout)) {}

    const VkShaderModule& getShaderModule() const { return m_shaderModule; }

    // Used in Pipeline creation
    VkPipelineShaderStageCreateInfo getPipelineShaderCreateInfo();

    // Used in Buffer binding
    // TODO(OS): We need to somhow let the Shader Stage control this by only providing which buffers bind to which set.
    void setDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>&);

    // Used in command recording
    void updateDescriptorSetLayout(std::vector<VkWriteDescriptorSet>);

  private:
    void createShaderModule();

    VkDevice& m_Device;
    const char* m_name = nullptr;
    const char* m_entryPoint = nullptr;
    std::filesystem::path m_path;
    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_DescriptorSetLayout;
};
}  // namespace RAYX
#endif