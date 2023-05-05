#ifndef NO_VULKAN
#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/Init/Descriptor.h"
#include "VulkanEngine/Init/ShaderStage.h"
namespace RAYX {
// General Pass
// -------------------------------------------------------------------------------------------------------
/**
 * @brief General Vulkan Pass (A group of Pipelines)
 *
 */
class RAYX_API Pass {
  public:
    // A series of Pipelines makes a Pass.
    // This is the node that contributes to creating a Pass
    class Pipeline {
      public:
        Pipeline(std::string name, VkDevice& dev, const ShaderStageCreateInfo&);
        ~Pipeline();

        void createPipelineLayout();
        void createPipeline();
        void inline readPipelineCache();
        void inline storePipelineCache(VkDevice& device);
        void cleanPipeline(VkDevice& device);

        std::string m_name;
        VkDevice& m_device;

        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
        VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;

        std::shared_ptr<ShaderStage> shaderStage{};
    };
    using Pipelines = std::vector<std::shared_ptr<Pipeline>>;

    Pass() = default;
    ~Pass() = default;

    void bind(const VkCommandBuffer& commandBuffer, int stage) const { vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline(stage)); }

    const char* getName() const { return m_name; }
    const Pipelines& getPass() const { return m_pass; }
    uint32_t getStageAmount() const { return m_stagesCount; }
    const VkPipeline getPipeline(int stage) const { return m_pass[stage]->m_pipeline; }
    const VkPipelineLayout getPipelineLayout(int stage) { return m_pass[stage]->m_pipelineLayout; }
    const ShaderStage& getShaderStage(int stage) const { return *(m_pass[stage]->shaderStage); }

    virtual const VkPipelineBindPoint& getPipelineBindPoint() const = 0;

  protected:
    // How many stages in the Pass
    uint32_t m_stagesCount;
    const char* m_name;
    // The pass itself (Pipelines)
    Pipelines m_pass = {VK_NULL_HANDLE};

    std::unique_ptr<DescriptorPool> globalDescriptorPool{};
};

// Compute Pass
// -------------------------------------------------------------------------------------------------------

/**
 * @brief Use ComputePipelineCreateInfo to build a new computePipeline
 *
 */
struct ComputePassCreateInfo {
    const char* passName;
    std::vector<ShaderStageCreateInfo> shaderStagesCreateInfos = {};
    int descriptorSetAmount = 1
};

/**
 * @brief Compute specified Pass:
 *
 * A compute Pass is the main class for executing a compute Program(Shader).
 * It mainly consists of which shader(s) to execute and each In-Output/Binding(s) corresponding to each stage stage*. \n
 *
 * - The ComputePass Class can have multiple Vkpipeline stages, i.e A ComputePass has a series of
 *  shader stages with barriers and other sync. methods in between.
 *  [Stage0->(Barrier)->Stage1 ..]
 */
class RAYX_API ComputePass : public Pass {
  public:
    explicit ComputePass(VkDevice& device, const ComputePassCreateInfo&);
    ~ComputePass();

    const VkPipelineBindPoint& getPipelineBindPoint() const { return m_PipelineBindPoint; }

    // Adding a new PipelineStage is the same as extending the Pass with a new Stage
    void addPipelineStage(const ShaderStageCreateInfo&);
    void addPipelineStage(const Pass& newStage);

    // Get bindings associated with the current Shader Module
    std::vector<VkDescriptorSetLayoutBinding> getDescriptorBindings();
    // Add/replace a binding
    // The buffer has to be an existent buffer!
    void addBufferBinding(uint32_t binding, const char* buffer);

  private:
    void createDescriptorPool();

    // Currently only one DescriptorSetLayout is offered per Pass
    void createDescriptorSetLayout();

    void createPipelines();

    VkDevice& m_Device;
    const char* m_name;

    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    VkPipelineBindPoint m_PipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;  // Always compute

    std::map<uint32_t, VkDescriptorSetLayoutBinding> m_DescriptorBindings;

    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts = {VK_NULL_HANDLE};  // For now, only one [0]
    std::vector<VkDescriptorSet> descriptorSets = {VK_NULL_HANDLE};                // For now, only one [0]
    VulkanEngine::pushConstants_t pushConstants = {nullptr};  // TODO (OS): Not really like this as every shader has its own push_t

    // TODO(OS): Add missing memory barriers
};
}  // namespace RAYX
#endif