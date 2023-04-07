#ifndef NO_VULKAN

#include "BufferHandler.h"
namespace RAYX {
BufferHandler::BufferHandler() {
    createTransferCommandBuffer();
    createTransferQueue();
    createStagingBuffer();
};

BufferHandler::~BufferHandler() {
    if (m_TransferCommandBuffer) {
        vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_TransferCommandBuffer);
    }

    // Destroy staging buffer
    vmaDestroyBuffer(m_VmaAllocator, m_StagingBuffer.getBuffer(), m_StagingBuffer.m_Alloca);

    // Destroy user-defined buffers
    for (auto& [name, buf] : m_Buffers) {
        vmaDestroyBuffer(m_VmaAllocator, buf.getBuffer(), buf.m_Alloca);
    }
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
    VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &m_TransferCommandBuffer));
}

void BufferHandler::createTransferQueue() { vkGetDeviceQueue(m_Device, m_computeFamily, 0, &m_TransferQueue); }

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
    auto f = m_Fences.transfer->fence();
    vkQueueSubmit(m_TransferQueue, 1, &submitInfo, *f);
}

void BufferHandler::createStagingBuffer() {
    m_StagingBuffer.m_VmaAllocator = m_VmaAllocator;

    VulkanBufferCreateInfo createInfo = {};
    createInfo.bufName = "Staging-buffer";
    createInfo.accessType = VKBUFFER_INOUT;
    createInfo.size = STAGING_SIZE;
    createInfo.binding = 0;

    m_StagingBuffer.m_createInfo = createInfo;
    m_StagingBuffer.createVmaBuffer(
        STAGING_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        m_StagingBuffer.m_Buffer, m_StagingBuffer.m_Alloca, &m_StagingBuffer.m_AllocaInfo,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
}

void BufferHandler::loadFromStagingBuffer(char* outdata, size_t bytes) {
    memcpy(outdata, m_StagingBuffer.getMappedMemory(), bytes);
    m_StagingBuffer.UnmapMemory();
}

void BufferHandler::storeToStagingBuffer(char* indata, size_t bytes) {
    memcpy(m_StagingBuffer.getMappedMemory(), indata, bytes);
    m_StagingBuffer.UnmapMemory();
}

void BufferHandler::waitTransferQueueIdle() { vkQueueWaitIdle(m_TransferQueue); }

void BufferHandler::readBufferRaw(const char* bufname, char* outdata, const VkQueue& computeQueue) {
    // if (m_state != EngineStates_t::POSTRUN) {
    //     RAYX_ERR << "you've forgotton to .run() the VulkanEngine. Thats "
    //                 "mandatory before reading it's output buffers.";
    // }

    auto b = m_Buffers[bufname];

    if (b.m_createInfo.accessType != VKBUFFER_OUT || b.m_createInfo.accessType != VKBUFFER_INOUT) {
        RAYX_ERR << "readBufferRaw(\"" << bufname << "\", ...) is not allowed, as \"" << bufname << "\" is not an output buffer";
    }

    size_t remainingBytes = b.getSize();
    size_t offset = 0;
    if (computeQueue != nullptr) {
        vkQueueWaitIdle(computeQueue);
    }
    vkQueueWaitIdle(m_TransferQueue);

    while (remainingBytes > 0) {
        size_t localbytes = std::min((size_t)STAGING_SIZE, remainingBytes);
        gpuMemcpy(m_StagingBuffer, 0, b, offset, localbytes);
        m_Fences.transfer->wait();
        loadFromStagingBuffer(outdata + offset, localbytes);
        offset += localbytes;
        remainingBytes -= localbytes;
    }
}

void BufferHandler::writeBufferRaw(const char* bufname, char* indata) {
    auto b = m_Buffers[bufname];

    if (b.m_createInfo.accessType != VKBUFFER_IN || b.m_createInfo.accessType != VKBUFFER_INOUT) {
        RAYX_ERR << "writeBufferRaw(\"" << bufname << "\", ...) is not allowed, as \"" << bufname << "\" has m_in = false";
    }

    size_t remainingBytes = b.getSize();
    size_t offset = 0;
    while (remainingBytes > 0) {
        size_t localbytes = std::min(remainingBytes, (size_t)STAGING_SIZE);
        storeToStagingBuffer(indata + offset, localbytes);
        gpuMemcpy(b, offset, m_StagingBuffer, 0, localbytes);

        offset += localbytes;
        remainingBytes -= localbytes;
        m_Fences.transfer->wait();
    }
}

template <typename T>
void BufferHandler::createBuffer(VulkanBufferCreateInfo createInfo, const std::vector<T>& vec) {
    auto name = createInfo.bufName;
    if (m_Buffers.find(name) != m_Buffers.end()) {
        RAYX_ERR << "Buffer " << name << " already exists. Try update func.";
        return;
    }

    auto newBuffer = VulkanBuffer(m_VmaAllocator, createInfo);

    m_Buffers[name] = std::move(newBuffer);

    if (vec) {
        writeBufferRaw(name, (char*)vec.data());
    }
}

template <typename T>
inline std::vector<T> BufferHandler::readBuffer(const char* bufname, bool indirect) {
    std::vector<T> out(m_buffers[bufname].size / sizeof(T));
    if (indirect) {
        readBufferRaw(bufname, (char*)out.data(), m_ComputeQueue);
    } else {
        readBufferRaw(bufname, (char*)out.data());
    }
    return out;
}

template <typename T>
void BufferHandler::updateBuffer(const char* bufname, const std::vector<T>& vec) {
    if (m_Buffers.find(bufname) == m_Buffers.end()) {
        RAYX_ERR << "Buffer " << bufname << " does not exist.";
        return;
    }

    if (vec) {
        writeBufferRaw(bufname, (char*)vec.data());
    }
}

void BufferHandler::freeBuffer(const char* bufname) {
    if (m_Buffers.find(bufname) == m_Buffers.end()) {
        RAYX_ERR << "Buffer " << bufname << " does not exist.";
        return;
    }
    vmaDestroyBuffer(m_VmaAllocator, m_Buffers[bufname].getBuffer(), m_Buffers[bufname].m_Alloca);
}

void BufferHandler::deleteBuffer(const char* bufname) {
    freeBuffer(bufname);
    m_Buffers.erase(bufname);
}

}  // namespace RAYX

#endif