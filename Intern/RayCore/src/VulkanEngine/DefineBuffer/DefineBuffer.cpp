#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::defineBufferByDataRaw(const char* bufname, GpuData data) {
    createBuffer(bufname, data.size());
    fillBuffer(bufname, data);
}

void VulkanEngine::defineBufferBySize(const char* bufname, VkDeviceSize size) {
    createBuffer(bufname, size);
}

}  // namespace RAYX
