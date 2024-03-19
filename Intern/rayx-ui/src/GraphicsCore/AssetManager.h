#pragma once

#include <unordered_map>
#include <string>
#include <filesystem>
#include <vulkan/vulkan.h>

class Device;
class Texture;

class AssetManager {
  public:
    AssetManager(const Device& device, VkDescriptorPool descriptorPool);
    ~AssetManager();

    template <typename T>
    VkDescriptorSet add(const std::string& id, const std::filesystem::path& path);

    template <typename T>
    std::shared_ptr<T> get(const std::string& id) const;

    void remove(const std::string& id);

    VkDescriptorSet getDescriptorSet(const std::string& id) const;

  private:
    const Device& m_Device;
    VkDescriptorPool m_descriptorPool;
    VkDescriptorSetLayout m_descriptorSetLayout;

    std::unordered_map<std::string, VkDescriptorSet> m_textureDescriptorSets;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

    void createDescriptorSetLayout();
    VkDescriptorSet createDescriptorSetForTexture(const Texture& texture);
};
