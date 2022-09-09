#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {

void VulkanEngine::run(RunSpec spec) {
	m_numberOfInvocations = spec.m_numberOfInvocations;
	
    createDescriptorSet();
    createComputePipeline();
    createCommandBuffer();
    runCommandBuffer();
}

}
