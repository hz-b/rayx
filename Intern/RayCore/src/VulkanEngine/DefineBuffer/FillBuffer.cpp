#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::fillBuffer(const char* bufname, GpuData data) {
	int offset = 0;
	for (auto dataFragment : data.raw) {
		storeToStagingBuffer(dataFragment);
		gpuMemcpy(m_stagingBuffer, 0, m_buffers[bufname].m_Buffer,
				  offset, dataFragment.size());
		offset += dataFragment.size();
	}
}

}  // namespace RAYX
