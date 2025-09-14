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

    Texture(const Texture&)            = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    VkImage getImage() const { return m_image; }
    VkImageView getView() const { return m_view; }
    VkSampler getSampler() const { return m_sampler; }
    VkDescriptorImageInfo getDescriptorInfo() const { return {m_sampler, m_view, m_layout}; }

    void updateFromData(const unsigned char* data, uint32_t width, uint32_t height);
    void updateFromPath(const std::filesystem::path& path);
    void resize(uint32_t width, uint32_t height);
    void transitionImageLayout(VkImageLayout newLayout);

    struct TextureInput {
        std::unique_ptr<unsigned char[]> data;
        uint32_t width;
        uint32_t height;

        // Default constructor
        TextureInput() = default;

        // Parameterized constructor
        TextureInput(std::unique_ptr<unsigned char[]> d, uint32_t w, uint32_t h) : data(std::move(d)), width(w), height(h) {}

        // Move constructor
        TextureInput(TextureInput&& other) noexcept : data(std::move(other.data)), width(other.width), height(other.height) {}

        // Move assignment operator
        TextureInput& operator=(TextureInput&& other) noexcept {
            if (this != &other) {
                data   = std::move(other.data);
                width  = other.width;
                height = other.height;
            }
            return *this;
        }

        // Delete copy constructor and copy assignment operator
        TextureInput(const TextureInput&)            = delete;
        TextureInput& operator=(const TextureInput&) = delete;
    };

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