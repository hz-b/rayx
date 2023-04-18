#ifndef NO_VULKAN

#include "VulkanEngine/Init/Descriptor.h"

#include "RayCore.h"
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
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &m_DescriptorPool));
}

DescriptorPool::~DescriptorPool() { vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr); }

void DescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &allocInfo, &descriptor))
}

void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
    vkFreeDescriptorSets(m_Device, m_DescriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
}

void DescriptorPool::resetPool() { vkResetDescriptorPool(m_Device, m_DescriptorPool, 0); }

// *************** Descriptor Writer *********************

DescriptorWriter::DescriptorWriter(VkDescriptorSetLayout& setLayout, DescriptorPool& pool) : setLayout{setLayout}, pool{pool} {}

DescriptorWriter& DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

bool DescriptorWriter::build(VkDescriptorSet& set) {
    bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
    if (!success) {
        return false;
    }
    overwrite(set);
    return true;
}

void DescriptorWriter::overwrite(VkDescriptorSet& set) {
    for (auto& write : writes) {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool.m_Device, writes.size(), writes.data(), 0, nullptr);
}
}  // namespace RAYX

#endif