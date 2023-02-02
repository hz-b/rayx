#include <filesystem>
#include <vector>

#include "CanonicalizePath.h"
#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
void VulkanEngine::createCache() {
    /* Try to read pipeline cache file if exists */
    std::vector<uint8_t> pipeline_data;

    // Cache is stored in OS TEMP
    auto tmpDir = std::filesystem::temp_directory_path();
    try {
        pipeline_data = readFile(tmpDir / "pipeline_cache.data");
    } catch (std::runtime_error& ex) {
        RAYX_WARN << "No pipeline cache found.";
    }

    /* Add initial pipeline cache data from the cached file */
    VkPipelineCacheCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    create_info.initialDataSize = pipeline_data.size();
    create_info.pInitialData = pipeline_data.data();

    /* Create Vulkan pipeline cache */
    VK_CHECK_RESULT(vkCreatePipelineCache(m_Device, &create_info, nullptr, &m_PipelineCache));
}
}  // namespace RAYX