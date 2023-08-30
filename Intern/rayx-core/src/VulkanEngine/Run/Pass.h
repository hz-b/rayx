#ifndef NO_VULKAN
#pragma once

#include <vulkan/vulkan.hpp>

#include "RAY-Core.h"
#include "VulkanEngine/Buffer/BufferHandler.h"
#include "VulkanEngine/Buffer/PushConstant.h"
#include "VulkanEngine/Init/Descriptor.h"
namespace RAYX {

// General Pass
// -------------------------------------------------------------------------------------------------------
/**
 * @brief General Vulkan Pass (A group of Pipelines)
 * Not to be used directly, look at ComputePass/GraphicsPass
 */
class RAYX_API Pass {
  public:
    // Used for ShaderStage creation
    struct PipelineCreateInfo {
        std::string name;                  // Pipeline name
        std::filesystem::path shaderPath;  // Path of the shader stage
        VkShaderStageFlagBits shaderType = VK_SHADER_STAGE_COMPUTE_BIT;
    };
    // A series of Pipelines makes a Pass.
    // This is the node that contributes to creating a Pass
    // We can refer to a pipeline also as ´stage´
    //
    // It is highly recommended to use Pipelines inside Pass objects!
    class RAYX_API Pipeline {
      public:
        Pipeline(VkDevice& dev, const PipelineCreateInfo&);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;

        void createPipelineLayout(VkDescriptorSetLayout* setLayouts);
        void createPipeline();
        void inline readPipelineCache();
        void inline storePipelineCache(VkDevice& device);
        void updatePushConstant(void* data, size_t size);
        void cleanPipeline(VkDevice& device);

        const VkShaderModule& getShaderModule() const { return m_shaderModule; }
        VkShaderStageFlagBits getShaderStageFlagBits() const { return m_shaderType; }

        std::string m_name;
        VkDevice& m_device;

        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
        VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;

        PushConstantHandler m_pushConstant;

      private:
        VkPipelineShaderStageCreateInfo getPipelineShaderCreateInfo();
        void createShaderModule();
        bool isCleaned = false;
        std::string m_shaderEntryPoint = "main";
        std::filesystem::path m_shaderPath;
        VkShaderStageFlagBits m_shaderType;
        VkShaderModule m_shaderModule;
    };

    using Pipelines = std::vector<std::unique_ptr<Pipeline>>;

    Pass() = default;
    virtual ~Pass() = default;

    void bind(const VkCommandBuffer& commandBuffer, int stage) const {
        vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getVkPipeline(stage));
    }

    Pipelines& getPipelines() { return m_pass; }
    uint32_t getStageAmount() const { return m_stagesCount; }
    VkPipeline getVkPipeline(int stage) const { return m_pass[stage]->m_pipeline; }
    VkPipelineLayout getVkPipelineLayout(int stage) { return m_pass[stage]->m_pipelineLayout; }
    DescriptorPool& getDescriptorPool() { return *m_globalDescriptorPool.get(); }

    void updatePushConstant(int stage, void* data, uint32_t size);

    virtual const VkPipelineBindPoint& getPipelineBindPoint() const = 0;
    virtual void prepare(std::vector<VkDescriptorSetLayoutBinding>) = 0;

  protected:
    uint32_t m_stagesCount;
    std::string m_name;
    Pipelines m_pass = {};  // The pass itself (Pipelines)
    std::unique_ptr<DescriptorPool> m_globalDescriptorPool{};
};

// Compute Pass
// -------------------------------------------------------------------------------------------------------

/**
 * @brief Use ComputePipelineCreateInfo to build a new computePipeline
 *
 */
struct ComputePassCreateInfo {
    const char* passName;
    std::vector<Pass::PipelineCreateInfo> pipelineCreateInfos = {};
    int descriptorSetAmount = 1;
};

/**
 * @brief Compute specified Pass:
 *
 * A compute Pass is the main class for executing a compute Program(Shader).
 * It mainly consists of which shader(s) to execute and each In-Output Buffer Binding(s) corresponding to each stage stage. \n
 *
 * - The ComputePass Class can have multiple VkPipeline stages, i.e A ComputePass has a series of
 *  shader stages with barriers and other sync. methods in between.
 *  [Stage0->(Barrier)->Stage1-> ..]
 */
class RAYX_API ComputePass : public Pass {
  public:
    explicit ComputePass(VkDevice& device, const ComputePassCreateInfo&);
    ~ComputePass() override;

    void prepare(std::vector<VkDescriptorSetLayoutBinding> bindings) override;
    void createDescriptorPool(uint32_t maxSets, uint32_t bufferCount);

    const VkPipelineBindPoint& getPipelineBindPoint() const { return m_PipelineBindPoint; }
    const std::vector<VkDescriptorSet>& getDescriptorSets() const { return m_descriptorSets; }
    const std::vector<VkDescriptorSetLayout>& getDescriptorSetLayouts() const { return m_descriptorSetLayouts; }
    std::string getName() const { return m_name; }

    void addPipelineStage(const PipelineCreateInfo&);
    void updateDescriptorSets(BufferHandler* bufferHandler);
    void simpleUpdateDescriptorSets(BufferHandler* bufferHandler);
    void bindDescriptorSet(const VkCommandBuffer& cmdBuffer, int stage);
    void cmdPushConstants(const VkCommandBuffer& cmdBuffer, int stage);
    void cleanPipeline(int stage);

  private:
    // Currently only one DescriptorSetLayout is offered per Pass
    void createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& bindings);

    VkDevice& m_Device;
    VkPipelineBindPoint m_PipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;  // Always compute

    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;  // TODO(OS): For now, only one [0]
    std::vector<VkDescriptorSet> m_descriptorSets;              // TODO(OS): For now, only one [0]
};
}  // namespace RAYX
#endif