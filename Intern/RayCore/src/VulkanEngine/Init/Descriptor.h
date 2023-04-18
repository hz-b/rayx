#ifndef NO_VULKAN

#pragma once
#include <vulkan/vulkan.hpp>

#include "RayCore.h"
namespace RAYX {
class RAYX_API DescriptorPool {
  public:
    class RAYX_API Builder {
      public:
        Builder(VkDevice& device) : m_Device{device} {}

        Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder& setMaxSets(uint32_t count);

        std::unique_ptr<DescriptorPool> build() const;

      private:
        VkDevice& m_Device;
        std::vector<VkDescriptorPoolSize> poolSizes{};
        uint32_t maxSets = 1000;
        VkDescriptorPoolCreateFlags poolFlags = 0;
    };

    DescriptorPool(VkDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    void allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void resetPool();

  private:
    VkDevice& m_Device;
    VkDescriptorPool m_DescriptorPool;

    friend class DescriptorWriter;
};

class RAYX_API DescriptorWriter {
  public:
    DescriptorWriter(VkDescriptorSetLayout& setLayout, DescriptorPool& pool);

    DescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
    DescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

    bool build(VkDescriptorSet& set);
    void overwrite(VkDescriptorSet& set);

  private:
    VkDescriptorSetLayout& setLayout;
    DescriptorPool& pool;
    std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace RAYX

#endif