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
        // Count is usually size of the total buffers needed.
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

    // Allocates a Descriptor Set
    void allocateDescriptor(VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorSet& descriptorSet) const;
    // Frees a Descriptor Set
    void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void resetPool();
    VkDescriptorPool m_DescriptorPool;

  private:
    VkDevice& m_Device;

    friend class DescriptorWriter;
};

class RAYX_API DescriptorWriter {
  public:
    DescriptorWriter(VkDescriptorSetLayout& setLayout, DescriptorPool& pool);

    // Write Buffer with correct binding. Writing can be understood as pointing or binding.
    // BufferInfo contains indication about the actual Vulkan Buffer
    DescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
    DescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

    // Allocate and update a Descriptor Set from Pool
    void build(VkDescriptorSet& set);
    // Update a Descriptor Set
    void overwrite(VkDescriptorSet& set);

  private:
    VkDescriptorSetLayout& setLayout;
    DescriptorPool& pool;
    std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace RAYX

#endif