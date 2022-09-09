#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::declareBuffer(const char* bufname, DeclareBufferSpec spec) {
    Buffer& b = m_buffers[bufname];
    b.m_in = spec.m_in;
    b.m_out = spec.m_out;
    b.m_binding = spec.m_binding;
}
}  // namespace RAYX
