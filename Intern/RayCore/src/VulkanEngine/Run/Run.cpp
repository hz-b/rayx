#ifndef NO_VULKAN

#include <algorithm>
#include <ranges>

#include "Tracer/TracerConfig.h"
#include "VulkanEngine/VulkanEngine.h"
namespace RAYX {

void VulkanEngine::updateDescriptorSets(std::string passName) {
    // m_ComputePass->updateDescriptorSets(m_BufferHandler);
    getComputePass(passName)->simpleUpdateDescriptorSets(m_BufferHandler);
}

void VulkanEngine::updateAllDescriptorSets() {
    for (const auto pass : m_computePasses) {
        updateDescriptorSets(pass->getName());
    }
}

bool allFinalized(const std::vector<RayMeta>& vector) {
    return std::ranges::all_of(vector, [](const RayMeta& element) { return element.finalized; });
}

static void printRayStats(const std::vector<Ray>& rayOut) {
    int _hit = 0;
    int _abs = 0;
    int _fly = 0;
    int _unin = 0;
    int _not = 0;

    for (const auto& r : rayOut) {
        if ((int)r.m_weight == (int)W_JUST_HIT_ELEM) {
            _hit++;
        } else if ((int)r.m_weight == (int)W_ABSORBED) {
            _abs++;
        } else if ((int)r.m_weight == (int)W_FLY_OFF) {
            _fly++;
        } else if ((int)r.m_weight == (int)W_UNINIT) {
            _unin++;
        } else if ((int)r.m_weight == (int)W_NOT_ENOUGH_BOUNCES) {
            _not++;
        }
    }
    // std::erase_if(rayOut, [&](auto& r) { return r.m_weight == W_UNINIT; });
    double hit = (double)(_hit) / rayOut.size();
    double abs = (double)(_abs) / rayOut.size();
    double fly = (double)(_fly) / rayOut.size();
    double unin = (double)(_unin) / rayOut.size();
    double notx = (double)(_not) / rayOut.size();
    RAYX_D_LOG << "_hit: " << hit << " _abs: " << abs << " _fly: " << fly << " _unin: " << unin << " _not: " << notx;
    RAYX_D_LOG << "===============";
}

std::vector<std::vector<Ray>> VulkanEngine::run(VulkanEngineRunSpec_t spec) {
    if (m_state == EngineStates_t::PREINIT) {
        RAYX_ERR << "you've forgotton to .init() the VulkanEngine";
    } else if (m_state == EngineStates_t::POSTRUN) {
        // RAYX_ERR << "you've forgotton to .cleanup() the VulkanEngine";
    }
    m_numberOfInvocations = spec.m_numberOfInvocations;
    const int maxBounces = spec.maxBounces;  // TODO(OS): Not here

    // Using new descriptor manager (TODO(OS): Fix new desc. manager)
    updateAllDescriptorSets();

    std::vector<std::vector<Ray>> _checkpoints;
    auto push0 = m_computePasses[0]->getPass()[0]->m_pushConstant.getActualPushConstant<PushConstants_t>();
    // TODO(OS): Maybe move this out of VulkanEngine.Run
    for (int i = 0; i < maxBounces; i++) {
        // Update PushConstant content
        push0->i_bounce = i;

        recordSimpleTraceCommand("singleTracePass", m_CommandBuffers[0], 0);
        submitCommandBuffer(0);
        VK_CHECK_RESULT(m_Fences.compute->waitAndReset())  // FIXME: Can be solved by another memory barrier in CommandBuffer

        auto rayOut = m_BufferHandler->readBuffer<Ray>("ray-buffer", true);
        auto rayMeta = m_BufferHandler->readBuffer<RayMeta>("ray-meta-buffer", true);

        _checkpoints.push_back(rayOut);

        printRayStats(rayOut);

        if (allFinalized(rayMeta)) {  // Are all rays finished?
            RAYX_VERB << "All finalized";
            break;
        }
    }

    auto push1 = m_computePasses[1]->getPass()[0]->m_pushConstant.getActualPushConstant<PushConstants_t>();
    push1->i_bounce = push0->i_bounce;

    recordSimpleTraceCommand("finalCollisionPass", m_CommandBuffers[0], 0);
    submitCommandBuffer(0);
    VK_CHECK_RESULT(m_Fences.compute->waitAndReset())
    auto rayOut = m_BufferHandler->readBuffer<Ray>("ray-buffer", true);
    RAYX_D_LOG << "Last:";
    printRayStats(rayOut);
    _checkpoints.push_back(rayOut);

    m_runs++;
    m_state = EngineStates_t::POSTRUN;
    return _checkpoints;
}

}  // namespace RAYX

#endif
