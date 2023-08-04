#ifndef NO_VULKAN

#pragma once
#include "VulkanEngine/VulkanEngine.h"
#include "vulkan/vulkan.hpp"
namespace RAYX {
/**
 * @brief Useful initializers for Vulkan to enhance readability.
 * Each type of helper function belongs to a namespace.
 * (Taken from Vulkan Samples)
 */
namespace VKINIT {

namespace Command {
inline VkCommandPoolCreateInfo command_pool_create_info() {
    VkCommandPoolCreateInfo command_pool_create_info{};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    return command_pool_create_info;
}

inline void create_command_pool(VkDevice device, VkCommandPoolCreateFlags flag, uint32_t queueFamilyIndex, VkCommandPool& commandPool) {
    auto info = command_pool_create_info();
    info.flags = flag;
    info.queueFamilyIndex = queueFamilyIndex;
    VK_CHECK_RESULT(vkCreateCommandPool(device, &info, nullptr, &commandPool));
}

inline VkCommandBufferBeginInfo command_buffer_begin_info() {
    VkCommandBufferBeginInfo cmdBufferBeginInfo{};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    return cmdBufferBeginInfo;
}
}  // namespace Command

/////////////////////////////////////////////////////////////////////////////////////////////

namespace RenderPass {
inline VkRenderPassBeginInfo render_pass_begin_info() {
    VkRenderPassBeginInfo render_pass_begin_info{};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    return render_pass_begin_info;
}

inline VkRenderPassCreateInfo render_pass_create_info() {
    VkRenderPassCreateInfo render_pass_create_info{};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    return render_pass_create_info;
}
}  // namespace RenderPass

/////////////////////////////////////////////////////////////////////////////////////////////

namespace Sync {
/** @brief Initialize an image memory barrier with no image transfer ownership */
inline VkImageMemoryBarrier image_memory_barrier() {
    VkImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    return image_memory_barrier;
}

/** @brief Initialize a buffer memory barrier with no image transfer ownership */
inline VkBufferMemoryBarrier buffer_memory_barrier() {
    VkBufferMemoryBarrier buffer_memory_barrier{};
    buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    buffer_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    buffer_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    return buffer_memory_barrier;
}

inline VkMemoryBarrier memory_barrier() {
    VkMemoryBarrier memory_barrier{};
    memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    return memory_barrier;
}
}  // namespace Sync

/////////////////////////////////////////////////////////////////////////////////////////////

namespace Resource {
inline VkImageCreateInfo image_create_info() {
    VkImageCreateInfo image_create_info{};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    return image_create_info;
}

inline VkSamplerCreateInfo sampler_create_info() {
    VkSamplerCreateInfo sampler_create_info{};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.maxAnisotropy = 1.0f;
    return sampler_create_info;
}

inline VkImageViewCreateInfo image_view_create_info() {
    VkImageViewCreateInfo image_view_create_info{};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    return image_view_create_info;
}

inline VkFramebufferCreateInfo framebuffer_create_info() {
    VkFramebufferCreateInfo framebuffer_create_info{};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    return framebuffer_create_info;
}
}  // namespace Resource

/////////////////////////////////////////////////////////////////////////////////////////////

namespace Descriptor {
inline VkDescriptorPoolCreateInfo descriptor_pool_create_info(uint32_t count, const VkDescriptorPoolSize* pool_sizes, uint32_t max_sets) {
    VkDescriptorPoolCreateInfo descriptor_pool_info{};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.maxSets = max_sets;
    descriptor_pool_info.poolSizeCount = count;
    descriptor_pool_info.pPoolSizes = pool_sizes;
    return descriptor_pool_info;
}

inline VkDescriptorPoolCreateInfo descriptor_pool_create_info(const std::vector<VkDescriptorPoolSize>& pool_sizes, uint32_t max_sets) {
    VkDescriptorPoolCreateInfo descriptor_pool_info{};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    descriptor_pool_info.pPoolSizes = pool_sizes.data();
    descriptor_pool_info.maxSets = max_sets;
    return descriptor_pool_info;
}

inline VkDescriptorPoolSize descriptor_pool_size(VkDescriptorType type, uint32_t count) {
    VkDescriptorPoolSize descriptor_pool_size{};
    descriptor_pool_size.type = type;
    descriptor_pool_size.descriptorCount = count;
    return descriptor_pool_size;
}

inline VkDescriptorSetLayoutBinding descriptor_set_layout_binding(VkDescriptorType type, VkShaderStageFlags flags, uint32_t binding,
                                                                  uint32_t count = 1) {
    VkDescriptorSetLayoutBinding set_layout_binding{};
    set_layout_binding.descriptorType = type;
    set_layout_binding.stageFlags = flags;
    set_layout_binding.binding = binding;
    set_layout_binding.descriptorCount = count;
    return set_layout_binding;
}

inline VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(const VkDescriptorSetLayoutBinding* bindings, uint32_t binding_count) {
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
    descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_create_info.pBindings = bindings;
    descriptor_set_layout_create_info.bindingCount = binding_count;
    return descriptor_set_layout_create_info;
}

inline VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
    descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_create_info.pBindings = bindings.data();
    descriptor_set_layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
    return descriptor_set_layout_create_info;
}
inline VkDescriptorSetAllocateInfo descriptor_set_allocate_info(VkDescriptorPool descriptor_pool, const VkDescriptorSetLayout* set_layouts,
                                                                uint32_t descriptor_set_count) {
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info{};
    descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_allocate_info.descriptorPool = descriptor_pool;
    descriptor_set_allocate_info.pSetLayouts = set_layouts;
    descriptor_set_allocate_info.descriptorSetCount = descriptor_set_count;
    return descriptor_set_allocate_info;
}

inline VkWriteDescriptorSet write_descriptor_set(VkDescriptorSet dst_set, VkDescriptorType type, uint32_t binding,
                                                 VkDescriptorBufferInfo* buffer_info, uint32_t descriptor_count = 1) {
    VkWriteDescriptorSet write_descriptor_set{};
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.pNext = nullptr;
    write_descriptor_set.dstSet = dst_set;
    write_descriptor_set.descriptorType = type;
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.pBufferInfo = buffer_info;
    write_descriptor_set.descriptorCount = descriptor_count;
    return write_descriptor_set;
}

inline VkWriteDescriptorSet write_descriptor_set(VkDescriptorSet dst_set, VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo* image_info,
                                                 uint32_t descriptor_count = 1) {
    VkWriteDescriptorSet write_descriptor_set{};
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstSet = dst_set;
    write_descriptor_set.descriptorType = type;
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.pImageInfo = image_info;
    write_descriptor_set.descriptorCount = descriptor_count;
    return write_descriptor_set;
}
}  // namespace Descriptor

/////////////////////////////////////////////////////////////////////////////////////////////

namespace Pipeline {
inline VkPipelineLayoutCreateInfo pipeline_layout_create_info(const VkDescriptorSetLayout* set_layouts, uint32_t set_layout_count = 1) {
    VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = set_layout_count;
    pipeline_layout_create_info.pSetLayouts = set_layouts;
    return pipeline_layout_create_info;
}

inline VkPipelineLayoutCreateInfo pipeline_layout_create_info(uint32_t set_layout_count = 1) {
    VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = set_layout_count;
    return pipeline_layout_create_info;
}

inline VkGraphicsPipelineCreateInfo pipeline_create_info(VkPipelineLayout layout, VkRenderPass render_pass, VkPipelineCreateFlags flags = 0) {
    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.layout = layout;
    pipeline_create_info.renderPass = render_pass;
    pipeline_create_info.flags = flags;
    pipeline_create_info.basePipelineIndex = -1;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    return pipeline_create_info;
}

inline VkGraphicsPipelineCreateInfo pipeline_create_info() {
    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.basePipelineIndex = -1;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    return pipeline_create_info;
}

inline VkComputePipelineCreateInfo compute_pipeline_create_info(VkPipelineLayout layout, VkPipelineShaderStageCreateInfo stage,
                                                                VkPipelineCreateFlags flags = 0) {
    VkComputePipelineCreateInfo compute_pipeline_create_info{};
    compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_pipeline_create_info.layout = layout;
    compute_pipeline_create_info.stage = stage;
    compute_pipeline_create_info.flags = flags;
    return compute_pipeline_create_info;
}
}  // namespace Pipeline

/////////////////////////////////////////////////////////////////////////////////////////////

namespace misc {
inline VkPushConstantRange push_constant_range(VkShaderStageFlags stage_flags, uint32_t size, uint32_t offset) {
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = stage_flags;
    push_constant_range.offset = offset;
    push_constant_range.size = size;
    return push_constant_range;
}

inline VkSubmitInfo submit_info() {
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    return submit_info;
}
}  // namespace misc
}  // namespace VKINIT
}  // namespace RAYX
#endif