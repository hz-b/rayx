#ifndef NO_VULKAN

#include "VulkanEngine/Init/Descriptor.h"

#include "RayCore.h"
#include "VulkanEngine/Init/Initializers.h"
#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
// *************** Descriptor Pool Builder *********************

DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count) {
    poolSizes.push_back({descriptorType, count});
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {
    poolFlags = flags;
    return *this;
}
DescriptorPool::Builder& DescriptorPool::Builder::setMaxSets(uint32_t count) {
    maxSets = count;
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
    return std::make_unique<DescriptorPool>(m_Device, maxSets, poolFlags, poolSizes);
}

// *************** Descriptor Pool *********************

DescriptorPool::DescriptorPool(VkDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags,
                               const std::vector<VkDescriptorPoolSize>& poolSizes)
    : m_Device{device} {
    auto info =
        VKINIT::Descriptor::descriptor_pool_create_info(static_cast<uint32_t>(poolSizes.size()), poolSizes.data(), maxSets);
    info.flags = poolFlags;

    VK_CHECK_RESULT(vkCreateDescriptorPool(device, &info, nullptr, &m_DescriptorPool));
}

DescriptorPool::~DescriptorPool() { vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr); }

void DescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
    auto allocInfo = VKINIT::Descriptor::descriptor_set_allocate_info(m_DescriptorPool, &descriptorSetLayout, 1);

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &allocInfo, &descriptor))
}

void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
    vkFreeDescriptorSets(m_Device, m_DescriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
}

void DescriptorPool::resetPool() { vkResetDescriptorPool(m_Device, m_DescriptorPool, 0); }

// *************** Descriptor Writer *********************

DescriptorWriter::DescriptorWriter(VkDescriptorSetLayout& setLayout, DescriptorPool& pool)
    : setLayout{setLayout}, pool{pool} {}

DescriptorWriter& DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
    // The Destination Set is filled @overwrite
    auto writeDescriptorSet =
        VKINIT::Descriptor::write_descriptor_set(nullptr, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, binding, bufferInfo, 1);

    writes.push_back(writeDescriptorSet);
    return *this;
}

DescriptorWriter& DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo) {
    auto writeDescriptorSet =
        VKINIT::Descriptor::write_descriptor_set(nullptr, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, binding, imageInfo, 1);

    writes.push_back(writeDescriptorSet);
    return *this;
}

void DescriptorWriter::build(VkDescriptorSet& set) {
    pool.allocateDescriptor(setLayout, set);

    overwrite(set);
}

void DescriptorWriter::overwrite(VkDescriptorSet& set) {
    for (auto& write : writes) {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool.m_Device, writes.size(), writes.data(), 0, nullptr);
}
}  // namespace RAYX

#endif