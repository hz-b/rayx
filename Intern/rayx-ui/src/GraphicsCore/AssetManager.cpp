#include "AssetManager.h"

#include "Device.h"
#include "Texture.h"

AssetManager::AssetManager(const Device& device, VkDescriptorPool descriptorPool) : m_Device(device), m_descriptorPool(descriptorPool) {
    createDescriptorSetLayout();
}

AssetManager::~AssetManager() { vkDestroyDescriptorSetLayout(m_Device.device(), m_descriptorSetLayout, nullptr); }

void AssetManager::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerLayoutBinding;

    vkCreateDescriptorSetLayout(m_Device.device(), &layoutInfo, nullptr, &m_descriptorSetLayout);
}

VkDescriptorSet AssetManager::createDescriptorSetForTexture(const Texture& texture) {
    VkDescriptorSet descriptorSet;

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;

    vkAllocateDescriptorSets(m_Device.device(), &allocInfo, &descriptorSet);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture.getImageView();
    imageInfo.sampler = texture.getSampler();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_Device.device(), 1, &descriptorWrite, 0, nullptr);

    return descriptorSet;
}

template <>
VkDescriptorSet AssetManager::add<Texture>(const std::string& id, const std::filesystem::path& path) {
    if (m_textures.find(id) == m_textures.end()) {
        // Texture loading
        auto texture = std::make_shared<Texture>(m_Device, path);
        m_textures[id] = texture;

        // Descriptor set creation for the texture
        auto descriptorSet = createDescriptorSetForTexture(*texture);
        // Assuming we have a map to store descriptor sets
        m_textureDescriptorSets[id] = descriptorSet;

        return descriptorSet;
    }
    RAYX_VERB << "Texture with id " << id << " already exists";
    return m_textureDescriptorSets[id];
}

template <>
std::shared_ptr<Texture> AssetManager::get<Texture>(const std::string& id) const {
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        return it->second;
    }
    return nullptr;
}

void AssetManager::remove(const std::string& id) { m_textures.erase(id); }

VkDescriptorSet AssetManager::getDescriptorSet(const std::string& id) const {
    auto it = m_textureDescriptorSets.find(id);
    if (it != m_textureDescriptorSets.end()) {
        return it->second;
    }
    return VK_NULL_HANDLE;
}
