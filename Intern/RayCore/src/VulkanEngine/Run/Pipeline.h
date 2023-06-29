#ifndef NO_VULKAN
#pragma once

#include <vulkan/vulkan.hpp>

#include "RayCore.h"
#include "VulkanEngine/Buffer/BufferHandler.h"
#include "VulkanEngine/Buffer/PushConstant.h"
#include "VulkanEngine/Init/Descriptor.h"
#include "VulkanEngine/Init/ShaderStage.h"
namespace RAYX {

// General Pass
// -------------------------------------------------------------------------------------------------------
/**
 * @brief General Vulkan Pass (A grouSp of Pipelines)
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

        void createPipelineLayout(VkDescriptorSetLayout* setLayouts);
        void createPipeline();
        void inline readPipelineCache();
        void inline storePipelineCache(VkDevice& device);
        void updatePushConstants(void* data, size_t size);
        void cleanPipeline(VkDevice& device);

        std::string m_name;
        VkDevice& m_device;

        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
        VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;

        PushConstant m_pushConstant;
        std::unique_ptr<ShaderStage> shaderStage{};
    };

    using Pipelines = std::vector<std::shared_ptr<Pipeline>>;

    Pass() = default;
    virtual ~Pass(){};

    void bind(const VkCommandBuffer& commandBuffer, int stage) const { vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline(stage)); }

    const Pipelines& getPass() const { return m_pass; }
    uint32_t getStageAmount() const { return m_stagesCount; }
    const VkPipeline& getPipeline(int stage) const { return m_pass[stage]->m_pipeline; }
    const VkPipelineLayout& getPipelineLayout(int stage) { return m_pass[stage]->m_pipelineLayout; }
    const ShaderStage& getShaderStage(int stage) const { return *m_pass[stage]->shaderStage.get(); }
    DescriptorPool* getDesriptorPool() { return globalDescriptorPool.get(); }

    void updatePushConstant(int stage, void* data, uint32_t size);

    virtual const VkPipelineBindPoint& getPipelineBindPoint() const = 0;
    virtual void prepare(std::vector<VkDescriptorSetLayoutBinding>) = 0;

  protected:
    // const char* m_name;
    // How many stages in the Pass
    uint32_t m_stagesCount;
    // The pass itself (Pipelines)
    Pipelines m_pass = {};

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
    int descriptorSetAmount = 1;
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

    void prepare(std::vector<VkDescriptorSetLayoutBinding> bindings);
    void createDescriptorPool(uint32_t maxSets, uint32_t bufferCount);

    const VkPipelineBindPoint& getPipelineBindPoint() const { return m_PipelineBindPoint; }
    const std::vector<VkDescriptorSet>& getDescriptorSets() const { return m_descriptorSets; }
    const std::vector<VkDescriptorSetLayout>& getDescriptorSetLayouts() const { return m_descriptorSetLayouts; }
    std::string getName() const { return m_name; }

    // Adding a new PipelineStage is the same as extending the Pass with a new Stage
    void addPipelineStage(const ShaderStageCreateInfo&);

    void updateDescriptorSets(BufferHandler* bufferHandler);
    void simpleupdate(BufferHandler* bufferHandler);

    void bindDescriptorSet(const VkCommandBuffer& cmdBuffer, int stage);
    void cmdPushConstants(const VkCommandBuffer& cmdBuffer, int stage);
    void cleanPipeline(int stage);

  private:
    // Currently only one DescriptorSetLayout is offered per Pass
    void createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& bindings);

    VkDevice& m_Device;
    std::string m_name;  // TODO(OS): Consider moving this to Pass parent class

    // VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;                        // FIXME(OS): Possibly not used
    VkPipelineBindPoint m_PipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;  // Always compute

    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;  // For now, only one [0]
    std::vector<VkDescriptorSet> m_descriptorSets;              // For now, only one [0]

    // TODO(OS): Add missing memory barriers
};
}  // namespace RAYX
#endif