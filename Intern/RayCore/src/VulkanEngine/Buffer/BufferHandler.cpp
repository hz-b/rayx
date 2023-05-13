#ifndef NO_VULKAN

#include "BufferHandler.h"

#include "VulkanEngine/Init/Initializers.h"

#define HANDLER_CHECK_BUFFER_EXIST(name)                                          \
    {                                                                             \
        if (m_Buffers.find(name) != m_Buffers.end()) {                            \
            RAYX_ERR << "Buffer " << name << " already exists. Try update func."; \
        }                                                                         \
    }
namespace RAYX {
BufferHandler::BufferHandler(VkDevice& device, VmaAllocator allocator, uint32_t queueFamilyIndex, size_t stagingSize)
    : m_Device(device), m_FamilyIndex(queueFamilyIndex), m_VmaAllocator(allocator), m_StagingSize(stagingSize) {
    VKINIT::Command::create_command_pool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_FamilyIndex, m_CommandPool);
    createTransferCommandBuffer();
    createTransferQueue();
    createStagingBuffer();
    createTransferFence();

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_TransferSemaphore);

}

BufferHandler::~BufferHandler() {
    if (m_TransferCommandBuffer) {
        vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_TransferCommandBuffer);
    }

    // Destroy staging buffer
    vmaDestroyBuffer(m_VmaAllocator, m_StagingBuffer->getBuffer(), m_StagingBuffer->m_Alloca);

    // Destroy user-defined buffers
    for (auto& [name, buf] : m_Buffers) {
        vmaDestroyBuffer(m_VmaAllocator, buf->getBuffer(), buf->m_Alloca);
    }
}

// std::vector<VkDescriptorSetLayoutBinding> BufferHandler::getVulkanBufferBindings() {
//     // TODO(OS) We need correct bindings and not all so enhance this by moving it to Shader
//     std::vector<VkDescriptorSetLayoutBinding> result;
//     result.reserve(m_Buffers.size());
//     for (const auto& x : m_Buffers) {
//         result.push_back(x.second.m_DescriptorSetLayoutBinding);
//     }
//     return result;
// }

std::vector<VkDescriptorSetLayoutBinding> BufferHandler::getDescriptorBindings(const std::string& pass) {
    RAYX_PROFILE_FUNCTION();
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    for (auto& [name, buf] : m_Buffers) {
        bindings.push_back(buf->m_DescriptorSetBindings[pass]);
    }

    return bindings;
}

void BufferHandler::createTransferCommandBuffer() {
    /*
    Allocate a command buffer from the previously creeated command pool.
    */
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_CommandPool;  // specify the command pool to allocate from.

    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;
    VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &m_TransferCommandBuffer))
}

void BufferHandler::createTransferFence() { m_TransferFence = std::make_unique<Fence>(m_Device); }

void BufferHandler::createTransferQueue() { vkGetDeviceQueue(m_Device, m_FamilyIndex, 0, &m_TransferQueue); }

void BufferHandler::gpuMemcpy(VulkanBuffer& buffer_dst, size_t offset_dst, VulkanBuffer& buffer_src, size_t offset_src, size_t bytes) {
    RAYX_PROFILE_FUNCTION();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(m_TransferCommandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = offset_src;
    copyRegion.dstOffset = offset_dst;
    copyRegion.size = bytes;

    vkCmdCopyBuffer(m_TransferCommandBuffer, buffer_src.getBuffer(), buffer_dst.getBuffer(), 1, &copyRegion);

    vkEndCommandBuffer(m_TransferCommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_TransferCommandBuffer;
    auto f = m_TransferFence->fence();
    vkQueueSubmit(m_TransferQueue, 1, &submitInfo, *f);
}

void BufferHandler::createStagingBuffer() {
    m_StagingBuffer->m_VmaAllocator = m_VmaAllocator;

    VulkanBufferCreateInfo createInfo = {};
    createInfo.bufName = "Staging-buffer";
    createInfo.accessType = VKBUFFER_INOUT;
    createInfo.size = m_StagingSize;

    m_StagingBuffer->m_createInfo = createInfo;
    m_StagingBuffer->createVmaBuffer(
        m_StagingSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        m_StagingBuffer->m_Buffer, m_StagingBuffer->m_Alloca, &m_StagingBuffer->m_AllocaInfo,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
}

void BufferHandler::loadFromStagingBuffer(char* outdata, size_t bytes) {
    memcpy(outdata, m_StagingBuffer->getMappedMemory(), bytes);
    m_StagingBuffer->UnmapMemory();
}

void BufferHandler::storeToStagingBuffer(char* indata, size_t bytes) {
    memcpy(m_StagingBuffer->getMappedMemory(), indata, bytes);
    m_StagingBuffer->UnmapMemory();
}

void BufferHandler::waitTransferQueueIdle() { vkQueueWaitIdle(m_TransferQueue); }

VulkanBuffer* BufferHandler::getBuffer(const std::string& name) {
    HANDLER_CHECK_BUFFER_EXIST(name)
    return (VulkanBuffer*)&m_Buffers[name];
}

void BufferHandler::readBufferRaw(const char* bufname, char* outdata, const VkQueue& queue) {
    // if (m_state != EngineStates_t::POSTRUN) {
    //     RAYX_ERR << "you've forgotton to .run() the VulkanEngine. Thats "
    //                 "mandatory before reading it's output buffers.";
    // }

    auto buffer = getBuffer(bufname);
    if (buffer->m_createInfo.accessType != VKBUFFER_OUT) {
        RAYX_ERR << "readBufferRaw(\"" << bufname << "\", ...) is not allowed, as \"" << bufname << "\" is not an output buffer";
    }

    size_t remainingBytes = buffer->getSize();
    size_t offset = 0;
    if (queue != nullptr) {
        vkQueueWaitIdle(queue);
    }
    vkQueueWaitIdle(m_TransferQueue);

    while (remainingBytes > 0) {
        size_t localbytes = std::min((size_t)m_StagingSize, remainingBytes);
        gpuMemcpy(*m_StagingBuffer, 0, *m_Buffers[bufname], offset, localbytes);
        m_TransferFence->wait();
        loadFromStagingBuffer(outdata + offset, localbytes);
        offset += localbytes;
        remainingBytes -= localbytes;
    }
}

void BufferHandler::writeBufferRaw(const char* bufname, char* indata) {
    auto b = getBuffer(bufname);

    if (b->m_createInfo.accessType != VKBUFFER_IN || b->m_createInfo.accessType != VKBUFFER_INOUT) {
        RAYX_ERR << "writeBufferRaw(\"" << bufname << "\", ...) is not allowed, as \"" << bufname << "\" has m_in = false";
    }

    size_t remainingBytes = b->getSize();
    size_t offset = 0;
    while (remainingBytes > 0) {
        size_t localbytes = std::min(remainingBytes, (size_t)m_StagingSize);
        storeToStagingBuffer(indata + offset, localbytes);
        gpuMemcpy(*b, offset, *m_StagingBuffer, 0, localbytes);

        offset += localbytes;
        remainingBytes -= localbytes;
        m_TransferFence->wait();
    }
}

/**
 * @brief  Creates an "empty" buffer
 *
 * @param createInfo
 */
VulkanBuffer& BufferHandler::createBuffer(VulkanBufferCreateInfo createInfo) {
    auto name = createInfo.bufName;
    HANDLER_CHECK_BUFFER_EXIST(name)
    m_Buffers[name] = std::make_unique<VulkanBuffer>(m_VmaAllocator, createInfo);
    return *m_Buffers[name];
}

// template <typename T>
// inline std::vector<T> BufferHandler::readBuffer(const char* bufname, bool indirect) {
//     std::vector<T> out(m_Buffers[bufname]->getSize() / sizeof(T));
//     if (indirect) {
//         readBufferRaw(bufname, (char*)out.data(), m_TransferQueue);
//     } else {
//         readBufferRaw(bufname, (char*)out.data());
//     }
//     return out;
// }

template <typename T>
void BufferHandler::updateBuffer(const char* bufname, const std::vector<T>& vec) {
    HANDLER_CHECK_BUFFER_EXIST(bufname)
    if (vec) {
        writeBufferRaw(bufname, (char*)vec.data());
    }
}

void BufferHandler::freeBuffer(const char* bufname) {
    HANDLER_CHECK_BUFFER_EXIST(bufname)
    vmaDestroyBuffer(m_VmaAllocator, m_Buffers[bufname]->getBuffer(), m_Buffers[bufname]->m_Alloca);
}

void BufferHandler::deleteBuffer(const char* bufname) {
    freeBuffer(bufname);
    m_Buffers.erase(bufname);
}

}  // namespace RAYX

#endif