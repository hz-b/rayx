#pragma once

#include <vulkan/vulkan.h>

#include <filesystem>
#include <memory>

#include "GraphicsCore/Device.h"

class Buffer;

class Texture {
  public:
    Texture(const Device& device);
    Texture(const Device& device, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectFlags, VkExtent2D extent);
    ~Texture();

    // Not copyable
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    // Movable
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    VkImage getImage() const { return m_image; }
    VkImageView getView() const { return m_view; }
    VkSampler getSampler() const { return m_sampler; }
    VkDescriptorImageInfo getDescriptorInfo() const { return {m_sampler, m_view, m_layout}; }

    struct TextureInput {
        std::unique_ptr<unsigned char[]> data;
        uint32_t width;
        uint32_t height;
    };

    void updateFromData(const unsigned char* data, uint32_t width, uint32_t height);
    void updateFromPath(const std::filesystem::path& path);
    void resize(uint32_t width, uint32_t height);
    void transitionImageLayout(VkImageLayout newLayout);

  private:
    const Device& m_Device;
    VkExtent2D m_extent;
    VkFormat m_format;

    VkImage m_image;
    VkDeviceMemory m_memory;
    VkImageView m_view;
    VkSampler m_sampler;
    VkImageLayout m_layout;
    VkImageUsageFlags m_usageFlags;
    VkImageAspectFlags m_aspectFlags;

    void setupTransitionDetails(VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags& aspectMask, VkAccessFlags& srcAccessMask,
                                VkAccessFlags& dstAccessMask, VkPipelineStageFlags& sourceStage, VkPipelineStageFlags& destinationStage);
    VkImageAspectFlags determineAspectMask() const;

    std::vector<uint8_t> dataFromPath(const std::filesystem::path& path);
    void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
    void createImage(VkImageTiling tiling, VkMemoryPropertyFlags properties);
    void createImageView();
    void createSampler();
    void cleanup();
};