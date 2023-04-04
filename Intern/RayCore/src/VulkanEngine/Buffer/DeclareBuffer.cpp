#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::declareBuffer(const char* bufname, BufferDeclarationSpec_t spec) {
    if (m_state != EngineStates_t::PREINIT) {
        RAYX_ERR << "VulkanEngine was already initialized!";
    }

    Buffer_t& b = m_buffers[bufname];
    b.isInput = spec.isInput;
    b.isOutput = spec.isOutput;
    b.binding = spec.binding;
}
}  // namespace RAYX

#endif
