#ifndef NO_VULKAN

#include "CanonicalizePath.h"
#include "Debug/Instrumentor.h"
#include "VulkanEngine/VulkanEngine.h"

namespace RAYX {
/**
 * @brief Create a shader module an assign it to Compute
 *
 */
void VulkanEngine::createShaderModule() {
    RAYX_PROFILE_FUNCTION_STDOUT();
    /*
    Create a shader module. A shader module basically just encapsulates some
    shader code.
    */

    // the code in comp.spv was created by running the command:
    // glslangValidator.exe -V shader.comp
    std::string path = getExecutablePath().string() + "/Shaders/comp.spv";
    RAYX_VERB << "Loading compute shader from: " << path;
    std::vector<uint32_t> compShaderCode;
    if (auto d = readFileAlign32(path)) {
        compShaderCode = d.value();
    } else {
        RAYX_ERR << "failed to load the shader!";
    }
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = compShaderCode.data();
    createInfo.codeSize = compShaderCode.size() * sizeof(uint32_t);

    RAYX_VERB << "Creating compute shader module..";

    VK_CHECK_RESULT(vkCreateShaderModule(m_Device, &createInfo, nullptr, &m_ComputeShaderModule))
    RAYX_VERB << "Shader module(s) created.";
}
}  // namespace RAYX

#endif
