#ifndef NO_VULKAN

#pragma once

#include "Core.h"
#include "Tracer/Tracer.h"
#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

class RAYX_API VulkanTracer : public Tracer {
  public:
    VulkanTracer() = default;
    ~VulkanTracer() = default;

    void listPhysicalDevices();
    std::vector<Ray> traceRaw(const TraceRawConfig&) override;
    void setPushConstants(const PushConstants*) override;
#ifdef RAYX_DEBUG_MODE
    /**
     * @brief Get the Debug List containing the Debug Matrices
     * (Size heavy)
     *
     * @return std::vector<..> of Debug Struct (MAT4x4)
     */
    auto getDebugList() const { return m_debugBufList; }
#endif

  private:
    VulkanEngine m_engine;

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
    // stf140 align rules (Stick to only 1 matrix for simplicity)
    struct debugBuffer_t {
        glm::mat4x4 _dMat;  // Set to identiy matrix in shader.
    };
    std::vector<debugBuffer_t> m_debugBufList;

    void initEngine();
};
}  // namespace RAYX

#endif
