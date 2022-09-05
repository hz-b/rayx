#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::initVk() {
    createInstance();
    setupDebugMessenger();
}
}  // namespace RAYX
