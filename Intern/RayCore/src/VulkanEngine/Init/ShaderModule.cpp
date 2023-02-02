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
    uint32_t filelength;
    // the code in comp.spv was created by running the command:
    // glslangValidator.exe -V shader.comp
    std::string path = canonicalizeRepositoryPath(m_shaderfile).string();
    uint32_t* compShaderCode = readFile(filelength, path.c_str());
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = compShaderCode;
    createInfo.codeSize = filelength;

    RAYX_VERB << "Creating compute shader module..";

    VK_CHECK_RESULT(vkCreateShaderModule(m_Device, &createInfo, nullptr, &m_ComputeShaderModule));
    RAYX_VERB << "Shader module(s) created.";
    delete[] compShaderCode;
}
}  // namespace RAYX