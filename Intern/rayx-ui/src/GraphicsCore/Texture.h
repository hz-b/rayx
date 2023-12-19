#pragma once

#include <vulkan/vulkan.h>

#include <filesystem>
#include <memory>

#include "GraphicsCore/Device.h"

class Buffer;

class Texture {
  public:
    Texture(Device& device, const std::filesystem::path& path);
    Texture(Device& device, const unsigned char* data, uint32_t width, uint32_t height);
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    ~Texture();

    VkImageView getImageView() const { return m_textureImageView; }
    VkSampler getSampler() const { return m_textureSampler; }
    std::shared_ptr<VkDescriptorImageInfo> descriptorInfo() { return m_imageInfo; }

  private:
    void init(const unsigned char* data, uint32_t width, uint32_t height);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties);
    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void createImageView(VkFormat format);
    void createTextureSampler();
    void cleanup();

    Device& m_Device;
    std::unique_ptr<Buffer> m_stagingBuffer;

    VkImage m_textureImage;
    VkDeviceMemory m_textureMemory;
    VkImageView m_textureImageView;
    VkSampler m_textureSampler;
    std::shared_ptr<VkDescriptorImageInfo> m_imageInfo;
};
