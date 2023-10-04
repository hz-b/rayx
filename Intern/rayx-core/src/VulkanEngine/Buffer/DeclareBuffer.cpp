#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::declareBuffer(const char* buffName, BufferDeclarationSpec spec) {
    if (m_state != EngineStates::PREINIT) {
        RAYX_ERR << "VulkanEngine was already initialized!";
    }

    Buffer& b = m_buffers[buffName];
    b.isInput = spec.isInput;
    b.isOutput = spec.isOutput;
    b.binding = spec.binding;
}
}  // namespace RAYX

#endif
