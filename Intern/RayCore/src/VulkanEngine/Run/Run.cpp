#ifndef NO_VULKAN

#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::updateDescriptorSets(std::string passName) {
    // m_ComputePass->updateDescriptorSets(m_BufferHandler);
    getComputePass(passName)->simpleupdate(m_BufferHandler);
}

void VulkanEngine::updateAllDescriptorSets() {
    for (const auto pass : m_computePasses) {
        updateDescriptorSets(pass->getName());
    }
}

bool allFinalized(const std::vector<RayMeta>& vector) {
    for (const auto& element : vector) {
        if (!element.finalized) {
            return false;
        }
    }
    return true;
}

std::vector<std::vector<Ray>> VulkanEngine::run(VulkanEngineRunSpec_t spec) {
    if (m_state == EngineStates_t::PREINIT) {
        RAYX_ERR << "you've forgotton to .init() the VulkanEngine";
    } else if (m_state == EngineStates_t::POSTRUN) {
        // RAYX_ERR << "you've forgotton to .cleanup() the VulkanEngine";
    }
    m_numberOfInvocations = spec.m_numberOfInvocations;
    const int maxBounces = spec.maxBounces;  // TODO: Not here

    // Using new descriptor manager (TODO: Fix new desc. manager)
    updateAllDescriptorSets();

    std::vector<std::vector<Ray>> snapshot;

    // TODO (OS) : Remove this
    struct push_constant_t {
        double rayIdStart;
        double numRays;
        double randomSeed;
        double maxSnapshots;  // FIXME(OS) : Only used by CPU
        int i_bounce;
    };

    for (int i = 0; i < maxBounces; i++) {
        RAYX_D_LOG << "Bounce : "<< i;
        // HACK (TODO(OS): Remove this)
        auto push = m_computePasses[0]->getPass()[0]->m_pushConstant.getData();
        push_constant_t* pushPtr = const_cast<push_constant_t*>(static_cast<const push_constant_t*>(push));
        pushPtr->i_bounce = i;

        recordSimpleTraceCommand(m_CommandBuffers[0]);
        submitCommandBuffer(0);
        m_Fences.compute->wait();  // FIXME: Can be solved by another memory barrier in CommandBuffer

        auto rayOut = m_BufferHandler->readBuffer<Ray>("ray-buffer", true);
        auto rayMeta = m_BufferHandler->readBuffer<RayMeta>("ray-meta-buffer", true);

        snapshot.push_back(rayOut);
        if (allFinalized(rayMeta)) {  // Are all rays finished?
            RAYX_VERB << "All finalized";
            break;
        }
    }
    m_runs++;
    m_state = EngineStates_t::POSTRUN;
    return snapshot;
}

}  // namespace RAYX

#endif
