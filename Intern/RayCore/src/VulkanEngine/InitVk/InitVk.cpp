#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::initVk() {
    createInstance();
    setupDebugMessenger();
    pickDevice();
	createCommandPool();
}

}  // namespace RAYX
