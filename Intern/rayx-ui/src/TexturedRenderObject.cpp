#include "TexturedRenderObject.h"

void TexturedRenderObject::updateTexture(const std::filesystem::path& path) {
    m_Texture = Texture(m_Device, path);
    createDescriptorSet();
    m_isTextured = true;
}

void TexturedRenderObject::updateTexture(const unsigned char* data, uint32_t width, uint32_t height) {
    if (data == nullptr) RAYX_ERR << "Texture data is null";
    m_Texture = Texture(m_Device, data, width, height);
    createDescriptorSet();
    m_isTextured = true;
}

void TexturedRenderObject::createDescriptorSet() {
    std::shared_ptr<VkDescriptorImageInfo> descrInfo = m_Texture.descriptorInfo();

    if (m_descrSet != VK_NULL_HANDLE) {  // Destroy the existing descriptor set
        vkFreeDescriptorSets(m_Device.device(), m_descriptorPool->getVkHandle(), 1, &m_descrSet);
        m_descrSet = VK_NULL_HANDLE;
    }

    DescriptorWriter writer(*m_setLayout, *m_descriptorPool);
    writer.writeImage(0, descrInfo.get());

    if (!writer.build(m_descrSet)) {
        RAYX_ERR << "Failed to build descriptor set for texture";
    }
}