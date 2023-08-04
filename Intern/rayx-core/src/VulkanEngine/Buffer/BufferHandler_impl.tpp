#ifndef NO_VULKAN
/**
 * Template definitions of BufferHandler class
 */
namespace RAYX {
template <typename T>
VulkanBuffer& BufferHandler::createBuffer(VulkanBufferCreateInfo createInfo, const std::vector<T>& vec) {
    // if (m_state == EngineStates_t::PREINIT) {
    //     RAYX_ERR << "you've forgotten to .init() the VulkanEngine";
    // } else if (m_state == EngineStates_t::POSTRUN) {
    //     RAYX_ERR << "you've forgotten to .cleanup() the VulkanEngine";
    // }
    auto bufName = std::string(createInfo.bufName);

    if (isBufferPresent(std::string(bufName))) {  // If buffer already exists, update (if it still has the same size)
        VulkanBuffer* b = getBuffer(bufName);
        if (b->m_createInfo.size == vec.size() * sizeof(T)) {
            writeBufferRaw(bufName.c_str(), (char*)vec.data());
            return *m_Buffers[bufName];
        } else {
            deleteBuffer(bufName.c_str());  // If size is different, delete and repeat
        }
    }

    createInfo.size = vec.size() * sizeof(T);

    if (!vec.empty()) {
        createBuffer(createInfo);
        writeBufferRaw(createInfo.bufName, (char*)vec.data());
    } else {
        RAYX_WARN << "No fill data provided for " << bufName;
        createBuffer(createInfo);
    }
    return *m_Buffers[bufName];
}

template <typename T>
inline std::vector<T> BufferHandler::readBuffer(const char* bufname, bool waitForQueue) {
    std::vector<T> out(m_Buffers[bufname]->getSize() / sizeof(T));
    if (waitForQueue) {
        readBufferRaw(bufname, (char*)out.data(), m_TransferQueue);
    } else {
        readBufferRaw(bufname, (char*)out.data());
    }
    return out;
}

template <typename T>
void BufferHandler::updateBuffer(const char* bufname, const std::vector<T>& vec) {
    if (!isBufferPresent(std::string(bufname))) {
        RAYX_ERR << "Buffer " << bufname << " does not exist";
    }
    if (vec && m_Buffers[std::string(bufname)]->getSize() == vec.size()) {
        writeBufferRaw(bufname, (char*)vec.data());
    } else {
        RAYX_WARN << "Size mismatch";
    }
}

}  // namespace RAYX

#endif