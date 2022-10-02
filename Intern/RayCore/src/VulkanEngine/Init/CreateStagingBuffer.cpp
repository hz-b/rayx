#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::createStagingBuffer() {
    createVkBuffer(STAGING_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_stagingBuffer, m_stagingMemory);
}

}  // namespace RAYX
