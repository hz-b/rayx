#include "PathResolver.h"
#include "VulkanEngine/VulkanEngine.h"

// Read file into array of bytes, and cast to uint32_t*, then return.
// The data has been padded, so that it fits into an array uint32_t.
uint32_t* readFile(uint32_t& length, const char* filename) {
    RAYX_PROFILE_FUNCTION();
    FILE* fp = fopen(filename, "rb");
    if (fp == nullptr) {
        printf("Could not find or open file: %s\n", filename);
    }

    // get file size.
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    if (filesize == -1) {
        RAYX_D_ERR << "Could not get file size.";
        return nullptr;
    }
    fseek(fp, 0, SEEK_SET);

    uint32_t filesizepadded = uint32_t(ceil(filesize / 4.0)) * 4;

    // read file contents.
    char* str = new char[filesizepadded];
    uint32_t readCount = fread(str, sizeof(char), filesize, fp);
    if (readCount != (uint32_t)filesize) {
        RAYX_D_LOG << readCount << " != " << filesize << "...";
        RAYX_D_ERR << "Errors while reading file: " << filename;
    }
    fclose(fp);

    // data padding.
    for (uint32_t i = filesize; i < filesizepadded; i++) {
        str[i] = 0;
    }

    length = filesizepadded;
    return (uint32_t*)str;
}

namespace RAYX {
void VulkanEngine::prepareRun(RunSpec r) {
    createDescriptorSet(r);

    // a compute pipeline needs to be created
    // the descriptors created earlier will be submitted here
    createComputePipeline();
    createCommandBuffer(r);
}

/* Descriptor sets need a pool to allocate from, which is crated here. */
void VulkanEngine::createDescriptorSet(RunSpec r) {
    RAYX_PROFILE_FUNCTION();
    /*
    one descriptor for each buffer
    */
    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPoolSize.descriptorCount = r.computeBuffersCount;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets =
        1;  // we need to allocate one descriptor sets from the pool.
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

    // create descriptor pool.
    VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo,
                                           nullptr, &m_DescriptorPool));

    /*
    With the pool allocated, we can now allocate the descriptor set.
    */
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool =
        m_DescriptorPool;  // pool to allocate from.
    descriptorSetAllocateInfo.descriptorSetCount =
        1;  // allocate a single descriptor set.
    descriptorSetAllocateInfo.pSetLayouts = &m_DescriptorSetLayout;

    // allocate descriptor set.
    VK_CHECK_RESULT(vkAllocateDescriptorSets(
        m_Device, &descriptorSetAllocateInfo, &m_DescriptorSet));

    // Populate every Compute Buffer/Descriptor and configure it.
    for (uint32_t i = 0; i < m_compute.m_Buffers.size(); i++) {
        // specify which buffer to use: input buffer
        VkDescriptorBufferInfo descriptorBufferInfo = {};
        descriptorBufferInfo.buffer = m_compute.m_Buffers[i];
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = m_compute.m_BufferSizes[i];

        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet =
            m_DescriptorSet;                // write to this descriptor set.
        writeDescriptorSet.dstBinding = i;  // write to the ist binding
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;  // update a single descriptor.
        writeDescriptorSet.descriptorType =
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;  // storage buffer.
        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

        // perform the update of the descriptor set.
        vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);
    }
}
/*
We create a compute pipeline here.
*/
void VulkanEngine::createComputePipeline() {
    RAYX_PROFILE_FUNCTION();

    /*
    Create a shader module. A shader module basically just encapsulates some
    shader code.
    */
    uint32_t filelength;
    // the code in comp.spv was created by running the command:
    // glslangValidator.exe -V shader.comp
    std::string path = resolvePath(m_initSpec->shaderfile);
    uint32_t* compShaderCode = readFile(filelength, path.c_str());
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = compShaderCode;
    createInfo.codeSize = filelength;

    RAYX_LOG << "Creating compute shader module..";

    VK_CHECK_RESULT(vkCreateShaderModule(m_Device, &createInfo, nullptr,
                                         &m_ComputeShaderModule));
    RAYX_LOG << "Shader module(s) created.";
    delete[] compShaderCode;

    RAYX_LOG << "Creating pipeline...";
    /*
    Now let us actually create the compute pipeline.
    It only consists of a single stage with a compute shader.
    So first we specify the compute shader stage, and it's entry
    point(main).
    */
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.module = m_ComputeShaderModule;
    shaderStageCreateInfo.pName = "main";

    /*
    The pipeline layout allows the pipeline to access descriptor sets.
    So we just specify the descriptor set layout we created earlier.
    */
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
    VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device, &pipelineLayoutCreateInfo,
                                           nullptr, &m_PipelineLayout));

    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stage = shaderStageCreateInfo;
    pipelineCreateInfo.layout = m_PipelineLayout;
    /*
    Now, we finally create the compute pipeline.
    */
    VK_CHECK_RESULT(vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1,
                                             &pipelineCreateInfo, nullptr,
                                             &m_Pipeline));
    RAYX_LOG << "Pipeline created.";
}

void VulkanEngine::createCommandBuffer(RunSpec r) {
    RAYX_PROFILE_FUNCTION();
    RAYX_LOG << "Creating commandBuffer..";
    /*
    Allocate a command buffer from the previously creeated command pool.
    */
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool =
        m_CommandPool;  // specify the command pool to allocate from.

    /* if the command buffer is primary, it can be directly submitted to
    / queues. A secondary buffer has to be called from some primary command
    / buffer, and cannot be directly submitted to a queue. To keep things
    / simple, we use a primary command buffer. */

    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount =
        1;  // allocate a single command buffer.
    VK_CHECK_RESULT(vkAllocateCommandBuffers(
        m_Device, &commandBufferAllocateInfo,
        &m_CommandBuffer));  // allocate command buffer.

    /*
    Now we shall start recording commands into the newly allocated command
    buffer.
    */
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags =
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;  // the buffer is only
                                                      // submitted and used
                                                      // once in this
                                                      // application.
    VK_CHECK_RESULT(vkBeginCommandBuffer(
        m_CommandBuffer, &beginInfo));  // start recording commands.

    /*
    We need to bind a pipeline, AND a descriptor set before we dispatch.
    The validation layer will NOT give warnings if you forget these, so be
    very careful not to forget them.
    */
    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                      m_Pipeline);
    vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                            m_PipelineLayout, 0, 1, &m_DescriptorSet, 0,
                            nullptr);
    // vkCmdBindDescriptorSets(commandBuffer,
    // VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1,
    // &descriptorSets[1], 0, NULL);

    /*
    Calling vkCmdDispatch basically starts the compute pipeline, and
    executes the compute shader. The number of workgroups is specified in
    the arguments. If you are already familiar with compute shaders from
    OpenGL, this should be nothing new to you.
    */
    auto requiredLocalWorkGroupNo =
        (uint32_t)ceil(r.numberOfInvocations /
                       float(WORKGROUP_SIZE));  // number of local works groups

    // check if there are too many rays
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

    auto limits = deviceProperties.limits;

    auto xgroups = limits.maxComputeWorkGroupCount[0];
    auto ygroups = limits.maxComputeWorkGroupCount[1];
    auto zgroups = limits.maxComputeWorkGroupCount[2];

    // if this limit is reached, you may start using
    // multidimensional local workgroups.
    if (requiredLocalWorkGroupNo > xgroups * ygroups * zgroups) {
        RAYX_ERR << "the given task requires " << requiredLocalWorkGroupNo
                 << " many local work groups, but the maximal number on this "
                    "machine is "
                 << xgroups * ygroups * zgroups;
    } else {
        RAYX_D_LOG << "your machine supports up to "
                   << xgroups * ygroups * zgroups * WORKGROUP_SIZE << " rays";
    }

    // decrease xgroups, ygroups, zgroups so that we get a small number of
    // workgroups stlil covering requiredLocalWorkGroupNo
    {
        while (xgroups * ygroups * (zgroups / 2) >= requiredLocalWorkGroupNo) {
            zgroups /= 2;
        }
        while (xgroups * ygroups * (zgroups - 1) >= requiredLocalWorkGroupNo) {
            zgroups--;
        }

        while (xgroups * (ygroups / 2) * zgroups >= requiredLocalWorkGroupNo) {
            ygroups /= 2;
        }
        while (xgroups * (ygroups - 1) * zgroups >= requiredLocalWorkGroupNo) {
            ygroups--;
        }

        while ((xgroups / 2) * ygroups * zgroups >= requiredLocalWorkGroupNo) {
            xgroups /= 2;
        }
        while ((xgroups - 1) * ygroups * zgroups >= requiredLocalWorkGroupNo) {
            xgroups--;
        }
    }

    RAYX_LOG << "Dispatching commandBuffer...";
    RAYX_D_LOG << "Sending "
               << "(" << xgroups << ", " << ygroups << ", " << zgroups
               << ") to the GPU";
    vkCmdDispatch(m_CommandBuffer, xgroups, ygroups, zgroups);

    VK_CHECK_RESULT(
        vkEndCommandBuffer(m_CommandBuffer));  // end recording commands.
}

}  // namespace RAYX
