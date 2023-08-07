#ifndef NO_VULKAN
#include "PushConstant.h"

#include "VulkanEngine/Init/Initializers.h"

namespace RAYX {
void PushConstantHandler::update(void* data, uint32_t size) {
    if (size > 258) {
        RAYX_WARN << "Using pushConstants bigger than 128 Bytes(" << size << ") might be unsupported on some GPUs. Check Compute Info";
    }

    m_data = data;
    m_size = size;
}

VkPushConstantRange PushConstantHandler::getVkPushConstantRange(VkShaderStageFlagBits flag, uint32_t offset) {
    return VKINIT::misc::push_constant_range(flag, m_size,
                                             offset);  // Can change Offset if some of the struct is to be ignored
}

}  // namespace RAYX
#endif