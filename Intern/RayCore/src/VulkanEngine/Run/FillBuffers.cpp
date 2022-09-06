#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::fillBuffers(RunSpec r) {
/*
    for (auto b : r.buffers) {
        auto ib = &m_internalBuffers[b.name];
		for (auto data : b.data) {
			data
		}
    }
	TODO
*/
}

/*
void VulkanEngine::fillStagingBuffer(std::string bufname, std::vector<char> data) {
    void* data;
    vkMapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[0], 0,
                m_engine.m_staging.m_BufferSizes[0], 0, &data);

	memcpy(((char*)data) + i * RAY_VECTOR_SIZE, (*raySetIterator).data(),
    vkUnmapMemory(m_engine.m_Device, m_engine.m_staging.m_BufferMemories[0]);
}
*/


}  // namespace RAYX
