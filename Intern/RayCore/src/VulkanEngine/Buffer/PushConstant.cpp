#ifndef NO_VULKAN
#include "PushConstant.h"

#include "VulkanEngine/Init/Initializers.h"

namespace RAYX {

void PushConstant::update(void* data, uint32_t size) {
    if (size < 258) {
        RAYX_WARN << "Using pushConstants bigger than 128 Bytes might be unsupported on some GPUs. Check Compute Info";
    }
    m_data = data;
    m_size = size;
}

VkPushConstantRange PushConstant::getVkPushConstantRange(VkShaderStageFlagBits flag, uint32_t offset) {
    VkPushConstantRange pushConstant = VKINIT::misc::push_constant_range(flag, m_size,
                                                                         0);  // Can change Offset if some of the struct is to be ignored
    pushConstant.offset = offset;
    return pushConstant;
}

}  // namespace RAYX
#endif