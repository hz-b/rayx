#ifndef NO_VULKAN
#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/Init/ShaderStage.h"
#include "VulkanEngine/VulkanEngine.h"
#include "VulkanEngine/Init/Descriptor.h"
namespace RAYX {
// General Pipeline
// -------------------------------------------------------------------------------------------------------

/**
 * @brief General Vulkan Pipeline
 *
 */
class RAYX_API Pipeline : protected VulkanEngine {
  public:
    // A series of Pipeline Stages makes a Pipeline pass.
    // This is the node that contributes to creating a Pass
    class PipelineStage {
      public:
        PipelineStage(std::string name, VkDevice& dev, const ShaderStageCreateInfo&, int descriptorSetAmount = 1);
        ~PipelineStage();

        std::string m_name;
        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
        VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts = {VK_NULL_HANDLE};  // For now, only one [0]
        std::vector<VkDescriptorSet> descriptorSets = {VK_NULL_HANDLE};                // For now, only one [0]
        VulkanEngine::pushConstants_t pushConstants = {nullptr};  // TODO (OS): Not really like this as every shader has its own push_t
        std::shared_ptr<ShaderStage> shaderStage{};
        VkDevice& m_device;

        void createDescriptorSetLayout();
        void createPipelineLayout();
        void createPipeline();
        void inline readPipelineCache();
        void inline storePipelineCache(VkDevice& device);
        void cleanPipeline(VkDevice& device) {
            vkDestroyPipeline(device, m_pipeline, nullptr);
            vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
            storePipelineCache(device);
        }
    };

    Pipeline() = default;
    ~Pipeline() = default;
    const char* m_name;

    void bind(const VkCommandBuffer& commandBuffer, int stage) const { vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline(stage)); }

    const std::vector<std::shared_ptr<PipelineStage>>& getPass() const { return m_pass; }
    const char* getName() const { return m_name; }
    uint32_t getStageAmount() const { return m_stagesCount; }

    // How many stages does the pass have?
    uint32_t m_stagesCount;

    // The pass itself (PipelineStages)
    std::vector<std::shared_ptr<PipelineStage>> m_pass = {VK_NULL_HANDLE};

    std::unique_ptr<DescriptorPool> globalPool{};

    virtual const VkPipelineBindPoint& getPipelineBindPoint() const = 0;
    virtual const VkPipeline getPipeline(int stage) const = 0;
    virtual const VkPipelineLayout getPipelineLayout(int stage) const = 0;
    virtual const ShaderStage& getShaderStage(int stage) const = 0;
    virtual void createDescriptorPool();
};

// Compute Pipeline
// -------------------------------------------------------------------------------------------------------

/**
 * @brief Use ComputePipelineCreateInfo to build a new computePipeline
 *
 */
struct ComputePipelineCreateInfo {
    const char* passName;
    std::vector<ShaderStageCreateInfo> shaderStagesCreateInfos = {};
};

/**
 * @brief Compute specified Pipeline
 *
 * A compute Pipeline is the main class for executing a compute Program(Shader).
 * It mainly consists of which shader(s) to execute and each In-Output/Binding(s) corresponding to each stage stage*. \n
 *
 * - The computePipeline Class can have multiple Vkpipeline stages, i.e A compute class has a pass that can be a series of
 *  shader stages with barriers in between.
 * [Stage0->(Barrier)->Stage1 ..]
 */
class RAYX_API ComputePipeline : public Pipeline {
  public:
    explicit ComputePipeline(const ComputePipelineCreateInfo&);
    ~ComputePipeline();

    const VkPipelineBindPoint& getPipelineBindPoint() const { return m_PipelineBindPoint; }
    const VkPipeline getPipeline(int stage) const { return m_pass[stage]->m_pipeline; }
    const VkPipelineLayout getPipelineLayout(int stage) { return m_pass[stage]->m_pipelineLayout; }
    const ShaderStage& getShaderStage(int stage) const { return *(m_pass[stage]->shaderStage); }

    // Adding a new PipelineStage is the same as extending the Pass with a new Stage
    void addPipelineStage(const ShaderStageCreateInfo&);
    void addPipelineStage(const PipelineStage& newStage);

  private:
    void createDescriptorPool();
    void createDescriptorSetLayout(int);
    void createDescriptorSetLayouts();
    void createPipelines();

    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    VkPipelineBindPoint m_PipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;  // Always compute
    // TODO(OS): Add missing memory barriers
};
}  // namespace RAYX
#endif