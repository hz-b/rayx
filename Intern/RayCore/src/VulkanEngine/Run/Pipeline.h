#ifndef NO_VULKAN
#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/VulkanEngine.h"
namespace RAYX {
// General Pipeline
// -------------------------------------------------------------------------------------------------------

/**
 * @brief General Vulkan Pipeline
 *
 */
class RAYX_API Pipeline : protected VulkanEngine {
  public:
    Pipeline() = default;
    ~Pipeline() = default;
    struct Pipeline_t {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
    };
    void bind(const VkCommandBuffer& commandBuffer, int stage) const { vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline(stage)); }

  private:
    virtual const VkPipelineBindPoint& getPipelineBindPoint() const = 0;
    virtual const VkPipeline getPipeline(int stage) const = 0;
    virtual const VkPipelineLayout getPipelineLayout(int stage) const = 0;
};

// Compute Pipeline
// -------------------------------------------------------------------------------------------------------

/**
 * @brief Use ComputePipelineCreateInfo to build a new computePipeline
 *
 */
struct ComputePipelineCreateInfo {
    const char* passName;
    std::vector<std::filesystem::path> computeShaderPaths;
    std::vector<const char*> entryPoints;
};

/**
 * @brief Compute specified Pipeline
 *
 * A compute Pipeline is the main class for executing a compute Program(Shader).
 * It mainly consists of which shader(s) to execute and each In-Output/Binding(s) corresponding to each stage stage*. \n
 *
 * -The computePipeline Class can have multiple Vkpipeline stages, i.e A compute class has a pass that can be a series of
 *  shader stages with barriers in between.
 * [Stage0->(Barrier)->Stage1 ..]
 */
class RAYX_API ComputePipeline : public Pipeline {
  public:
    explicit ComputePipeline(ComputePipelineCreateInfo);
    ~ComputePipeline();

    const VkPipelineBindPoint& getPipelineBindPoint() const { return m_PipelineBindPoint; }
    const std::vector<Pipeline_t>& getPipelines() const { return m_Pipelines; }
    const VkPipeline getPipeline(int stage) { return m_Pipelines[stage].pipeline; };
    const VkPipelineLayout getPipelineLayout(int stage) { return m_Pipelines[stage].pipelineLayout; };

  private:
    const char* m_name = nullptr;

    struct ComputeShader_t {
        const char* entryPoint;
        std::filesystem::path path;
        VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    };
    void createPipelineLayout();
    void createShaderModule(const char* entryPoint, const VkShaderModule& shaderModule);
    void createDescriptorPool();
    void createDescriptorLayout();
    void createDescriptorSetLayout();
    

    uint32_t m_stagesCount;

    std::vector<ComputeShader_t> m_shaderModules;
    std::vector<Pipeline_t> m_Pipelines;

    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipelineBindPoint m_PipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;  // Always compute

    VulkanEngine::pushConstants_t pushConstants;  // TODO (OS): Not really like this as every shader has its own push

    // TODO: Add missing memory barriers
};
}  // namespace RAYX
#endif