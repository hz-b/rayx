#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::fillBuffers(RunSpec r) {
    for (auto [name, data] : r.buffers) {
        int offset = 0;
        for (auto dataFragment : data.raw) {
            storeToStagingBuffer(dataFragment);
            gpuMemcpy(m_stagingBuffer, 0, m_internalBuffers[name].m_Buffer,
                      offset, dataFragment.size());
            offset += dataFragment.size();
        }
    }
}

}  // namespace RAYX
