#ifndef NO_VULKAN

#include "BufferHandler.h"

#include "VulkanEngine/Init/Initializers.h"

namespace RAYX {
/**
 * @brief Construct a new Buffer Handler object (It is recommended to have one Handler/Vulk.Instance)
 *
 * @param device
 * @param allocator
 * @param queueFamilyIndex
 * @param stagingSize use getStagingSize
 */
BufferHandler::BufferHandler(VkDevice& device, VmaAllocator allocator, uint32_t queueFamilyIndex, size_t stagingSize)
    : m_Device(device), m_FamilyIndex(queueFamilyIndex), m_VmaAllocator(allocator), m_StagingSize(stagingSize) {
    VKINIT::Command::create_command_pool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_FamilyIndex, m_CommandPool);
    createTransferCommandBuffer();
    createTransferQueue();  // Simply get a new queue similar to src queue for transfers
    createStagingBuffer();
    createTransferFence();
    createTransferSemaphore();
}

BufferHandler::~BufferHandler() {
    vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_TransferCommandBuffer);
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    vkDestroySemaphore(m_Device, m_TransferSemaphore, nullptr);
}

std::vector<VkDescriptorSetLayoutBinding> BufferHandler::getDescriptorBindings(const std::string& pass) {
    RAYX_PROFILE_FUNCTION();
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (auto& [name, buf] : m_Buffers) {
        bindings.push_back(buf->m_DescriptorSetBindings[pass]);
    }
    return bindings;
}
/**
 * @brief Allocate a command buffer from the created command pool.
 */
void BufferHandler::createTransferCommandBuffer() {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_CommandPool;  // specify the command pool to allocate from.
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;
    VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &m_TransferCommandBuffer))
}

void BufferHandler::createTransferFence() { m_TransferFence = std::make_unique<Fence>(m_Device); }

void BufferHandler::createTransferQueue() { vkGetDeviceQueue(m_Device, m_FamilyIndex, 0, &m_TransferQueue); }

void BufferHandler::createTransferSemaphore() {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_TransferSemaphore);
}

/// Copies data from one buffer to the other with given offsets.
/// This is used for the buffer <-> staging buffer communication
/// Careful : This is not an awaiting command so make sure to check the according fence transfer
/// or Queue Idle before copying again
/// {read,write}BufferRaw.
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

void BufferHandler::insertBufferMemoryBarrier(const std::string& bufferName, const VkCommandBuffer& commandBuffer, VkAccessFlags srcAccessMask,
                                              VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                              VkDeviceSize offset) {
    auto buffer = getBuffer(bufferName);

    VkBufferMemoryBarrier bufferMemoryBarrier = VKINIT::Sync::buffer_memory_barrier();
    bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    bufferMemoryBarrier.srcAccessMask = srcAccessMask;
    bufferMemoryBarrier.dstAccessMask = dstAccessMask;
    bufferMemoryBarrier.buffer = buffer->m_Buffer;
    bufferMemoryBarrier.offset = offset;
    bufferMemoryBarrier.size = buffer->getSize();  // FIXME(OS): Only available size is bound
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
}

void BufferHandler::createStagingBuffer() {
    VulkanBufferCreateInfo createInfo = {};
    createInfo.bufName = "Staging-buffer";
    createInfo.accessType = VKBUFFER_INOUT;
    createInfo.size = 0;  // Setting size to 0 to avoid unwanted creation

    m_StagingBuffer = std::make_unique<VulkanBuffer>(m_VmaAllocator, createInfo);

    // Manual creation
    m_StagingBuffer->createVmaBuffer(
        m_StagingSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        m_StagingBuffer->m_Buffer, m_StagingBuffer->m_Alloca, &m_StagingBuffer->m_AllocaInfo,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
}

/// loads `bytes` many bytes from the staging buffer into `outdata`.
void BufferHandler::loadFromStagingBuffer(char* outdata, size_t bytes) {
    memcpy(outdata, m_StagingBuffer->getMappedMemory(), bytes);
    m_StagingBuffer->UnmapMemory();
}

/// writes `bytes` many bytes from `indata` into the staging buffer.
void BufferHandler::storeToStagingBuffer(char* indata, size_t bytes) {
    memcpy(m_StagingBuffer->getMappedMemory(), indata, bytes);
    m_StagingBuffer->UnmapMemory();
}

void BufferHandler::waitTransferQueueIdle() {
    RAYX_D_LOG << "Waiting for transfer Queue...";
    vkQueueWaitIdle(m_TransferQueue);
}
/**
 * @brief Get defined/allocated buffer
 *
 * @param name buffer name
 * @return VulkanBuffer*
 */
VulkanBuffer* BufferHandler::getBuffer(const std::string& name) {
    if (!isBufferPresent(std::string(name))) {
        RAYX_ERR << "Buffer " << name << " does not exist";
    }
    return m_Buffers[name].get();
}

/// Reads a buffer and writes the data to `outdata`.
/// The full buffer is read.
/// This function uses the staging Buffer to read the data in chunks of
/// STAGING_SIZE. Only allowed for buffers with as OUT usage.
/// If queue -> Wait for queue to finish before reading
void BufferHandler::readBufferRaw(const char* bufname, char* outdata, const VkQueue& queue) {
    // if (m_state != EngineStates_t::POSTRUN) {
    //     RAYX_ERR << "you've forgotton to .run() the VulkanEngine. Thats "
    //                 "mandatory before reading it's output buffers.";
    // }
    VulkanBuffer* buffer = getBuffer(bufname);
    auto access = buffer->m_createInfo.accessType;
    if (access != VKBUFFER_OUT && access != VKBUFFER_INOUT) {
        RAYX_ERR << "readBufferRaw(\"" << bufname << "\", ...) is not allowed, as \"" << bufname << "\" is not an output buffer";
    }

    size_t remainingBytes = buffer->getSize();
    size_t offset = 0;

    if (queue != nullptr) {
        vkQueueWaitIdle(queue);
    }

    while (remainingBytes > 0) {
        size_t localBytes = std::min((size_t)m_StagingSize, remainingBytes);
        gpuMemcpy(*m_StagingBuffer, 0, *m_Buffers[bufname], offset, localBytes);
        m_TransferFence->waitAndReset();
        loadFromStagingBuffer(outdata + offset, localBytes);
        offset += localBytes;
        remainingBytes -= localBytes;
    }
}
/// writes the `indata` to the buffer.
/// It will write the full buffer size bytes.
/// this function uses staging buffer to write the data in chunks of
/// STAGING_SIZE. only allowed for buffers with `IN as usage.
void BufferHandler::writeBufferRaw(const char* bufname, char* indata) {
    VulkanBuffer* buffer = getBuffer(bufname);

    auto access = buffer->m_createInfo.accessType;
    if (access != VKBUFFER_IN && access != VKBUFFER_INOUT) {
        RAYX_ERR << "writeBufferRaw(\"" << bufname << "\", ...) is not allowed, as \"" << bufname << "\" is not an input Buffer";
    }
    size_t remainingBytes = buffer->getSize();
    size_t offset = 0;

    while (remainingBytes > 0) {
        size_t localBytes = std::min(remainingBytes, (size_t)m_StagingSize);
        storeToStagingBuffer(indata + offset, localBytes);
        gpuMemcpy(*buffer, offset, *m_StagingBuffer, 0, localBytes);
        offset += localBytes;
        remainingBytes -= localBytes;
        m_TransferFence->waitAndReset();
    }
}

/**
 * @brief  Creates an "empty" buffer and return it
 *
 * @param createInfo
 */
VulkanBuffer& BufferHandler::createBuffer(VulkanBufferCreateInfo createInfo) {
    auto name = createInfo.bufName;

    if (isBufferPresent(std::string(name))) {
        return *m_Buffers[name];
    }

    m_Buffers[name] = std::make_unique<VulkanBuffer>(m_VmaAllocator, createInfo);
    return *m_Buffers[name];
}

template <typename T>
void BufferHandler::updateBuffer(const char* bufname, const std::vector<T>& vec) {
    if (!isBufferPresent(std::string(bufname))) {
        RAYX_ERR << "Buffer " << bufname << " does not exist";
    }
    if (vec && m_Buffers[std::string(bufname)]->getSize() == vec.size()) {
        writeBufferRaw(bufname, (char*)vec.data());
    } else
        RAYX_WARN << "Size mismatch";
}

void BufferHandler::deleteBuffer(const char* bufname) {
    if (!isBufferPresent(std::string(bufname))) {
        RAYX_ERR << "Buffer " << bufname << " does not exist";
    }
    m_Buffers.erase(bufname);
}

bool BufferHandler::isBufferPresent(const std::string& name) { return m_Buffers.find(name) != m_Buffers.end(); }

}  // namespace RAYX

#endif