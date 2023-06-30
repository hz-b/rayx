#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::updateDescriptorSets(std::string passName) {
    // m_ComputePass->updateDescriptorSets(m_BufferHandler);
    getComputePass(passName)->simpleupdate(m_BufferHandler);
}

void VulkanEngine::run(VulkanEngineRunSpec_t spec) {
    if (m_state == EngineStates_t::PREINIT) {
        RAYX_ERR << "you've forgotton to .init() the VulkanEngine";
    } else if (m_state == EngineStates_t::POSTRUN) {
        // RAYX_ERR << "you've forgotton to .cleanup() the VulkanEngine";
    }
    m_numberOfInvocations = spec.m_numberOfInvocations;

    // Using new descriptor manager (TODO: Fix new desc. manager)
    updateDescriptorSets("InitTracePass");
    updateDescriptorSets("TracePass");
    // TODO: If needed, update pushconstants again

    int maxBounces = UINT_MAX; // TODO: Undefined
    recordFirstCommand();  // Init and trace 1 bounce
    submitCommandBuffer(0);
    m_Fences.compute->wait();  // FIXME: Can be solved by another memory barrier in CommandBuffer

    // Call snapshot on res/out vector with mem. barrier (srcMask : compute and destMask : transfer)
    // Something like this (With the barrier, the transfer will only start once the previous compute(pipeline) is done)
    // std::vector<Ray> out = bufferHandler->readBuffer<Ray>("output-buffer", true);
    // {
    // void takeSnapshot(Ray r, double w) <-- On all 'out' vector 
    // PS: It is possible to copy 'out', start second command and dispatch and then manage snapshot (async : avoid idle time)
    //}

    for (int i = 1; i < maxBounces; i++) {
        recordSecondCommand(); 
        submitCommandBuffer(1);
        m_Fences.compute->wait();  // FIXME: Can be solved by another memory barrier in CommandBuffer
        // Another snapshot! (Same procedure with possibility to start next loop index)
    }
    m_runs++;
    m_state = EngineStates_t::POSTRUN;
}

}  // namespace RAYX

#endif
