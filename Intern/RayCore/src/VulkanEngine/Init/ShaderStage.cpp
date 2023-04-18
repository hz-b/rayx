#ifndef NO_VULKAN
#pragma once
#include "VulkanEngine/Init/ShaderStage.h"

#include "CanonicalizePath.h"
#include "Debug/Instrumentor.h"
namespace RAYX {
ShaderStage::ShaderStage(VkDevice& device, const ShaderStageCreateInfo& createInfo)
    : m_Device(device), m_name(createInfo.name.c_str()), m_entryPoint(createInfo.entryPoint.c_str()), m_path(createInfo.shaderPath) {
    RAYX_LOG << "Creating shader stage";
    createShaderModule();
}

ShaderStage::~ShaderStage() {
    vkDestroyShaderModule(m_Device, m_shaderModule, nullptr);
}

void ShaderStage::createShaderModule() {
    RAYX_PROFILE_FUNCTION_STDOUT();
    /*
    Create a shader module. A shader module basically just encapsulates some
    shader code.
    */
    uint32_t filelength;
    // the code in comp.spv was created by running the command:
    // glslangValidator.exe -V shader.comp
    std::string path = canonicalizeRepositoryPath(m_path).string();
    uint32_t* compShaderCode = readFile(filelength, path.c_str());
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = compShaderCode;
    createInfo.codeSize = filelength;

    VK_CHECK_RESULT(vkCreateShaderModule(m_Device, &createInfo, nullptr, &m_shaderModule));
    RAYX_VERB << "Shader module " << m_name << " created.";
    delete[] compShaderCode;
}

VkPipelineShaderStageCreateInfo ShaderStage::getPipelineShaderCreateInfo() {
    /* we specify the compute shader stage, and it's entry
    point(main).
    */
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.module = m_shaderModule;
    shaderStageCreateInfo.pName = m_entryPoint;
    return shaderStageCreateInfo;
}

}  // namespace RAYX
#endif