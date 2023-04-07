#ifndef NO_VULKAN
#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/VulkanEngine.h"
namespace RAYX {
/**
 * @brief General Vulkan Pipeline
 *
 */
class RAYX_API Pipeline : protected VulkanEngine {
  public:
    Pipeline() = default;
    ~Pipeline() = default;
    void bind(const VkCommandBuffer& commandBuffer) const { vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline()); }

  private:
    virtual const VkPipelineBindPoint& getPipelineBindPoint() const = 0;
    virtual const VkPipeline& getPipeline() const = 0;
};
/**
 * @brief Compute specified class
 *
 */
class RAYX_API ComputePipeline : public Pipeline {
  public:
    explicit ComputePipeline();
    ~ComputePipeline();

    const VkPipelineBindPoint& getPipelineBindPoint() { return m_PipelineBindPoint; }
    const VkPipeline& getPipeline() { return m_Pipeline[0]; }

  private:
    struct ComputeShader_t {
        const char* entryPoint;
        std::filesystem::path path;
    };
    VulkanEngine::pushConstants_t pushConstants;  // TODO (OS): Not really like this as every shader has its own push

    void createPipelineLayout();
    void createShaderModule();
    void CreateDescriptorPool();
    void createDescriptorLayout();

    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    uint32_t m_stagesCount;
    std::vector<ComputeShader_t> m_shaderModules;
    std::vector<VkPipeline> m_Pipeline;
    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipelineBindPoint m_PipelineBindPoint;
    // TODO: Add missing memory barriers
};
}  // namespace RAYX
#endif