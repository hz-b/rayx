#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Buffer.h"

Texture::Texture(Device& device, const std::filesystem::path& path) : m_Device{device} {
    // Read in image
    int texWidth, texHeight, texChannels;  // Do not use texChannels for memory stuff (not always 4 for rgba images)
    RAYX_LOG << "Loading texture: " << path.string();
    // STBI_rgb_alpha hopefully always forces 4 channels
    stbi_uc* pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (!pixels) {
        RAYX_ERR << "Failed to load texture image";
    }

    init(pixels, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    stbi_image_free(pixels);
}

Texture::Texture(Device& device, const unsigned char* data, uint32_t width, uint32_t height) : m_Device{device} { init(data, width, height); }

Texture::Texture(Texture&& other) noexcept
    : m_Device(other.m_Device),
      m_stagingBuffer(std::move(other.m_stagingBuffer)),
      m_textureImage(other.m_textureImage),
      m_textureMemory(other.m_textureMemory),
      m_textureImageView(other.m_textureImageView),
      m_textureSampler(other.m_textureSampler),
      m_imageInfo(other.m_imageInfo) {
    // Set the source instance's resource handles to VK_NULL_HANDLE to denote they are no longer owned.
    other.m_textureImage = VK_NULL_HANDLE;
    other.m_textureMemory = VK_NULL_HANDLE;
    other.m_textureImageView = VK_NULL_HANDLE;
    other.m_textureSampler = VK_NULL_HANDLE;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        // Release any resources this instance owns
        cleanup();

        // Transfer ownership of resources
        if (m_Device.device() != other.m_Device.device()) {
            // If this fails you hopefully know what you're doing. I would not recommend using multiple devices.
            // If you don't know what's going on, you need to check why the devices are different.
            // If unhandled this will result in bugs and/or crashes.
            RAYX_WARN << "Cannot transfer ownership of resources between logical devices. Failing gracefully.";
            return *this;
        }
        m_stagingBuffer = std::move(other.m_stagingBuffer);
        m_textureImage = other.m_textureImage;
        m_textureMemory = other.m_textureMemory;
        m_textureImageView = other.m_textureImageView;
        m_textureSampler = other.m_textureSampler;
        m_imageInfo = other.m_imageInfo;

        // Set the source instance's resource handles to VK_NULL_HANDLE so they are not cleaned up (we moved them)
        other.m_textureImage = VK_NULL_HANDLE;
        other.m_textureMemory = VK_NULL_HANDLE;
        other.m_textureImageView = VK_NULL_HANDLE;
        other.m_textureSampler = VK_NULL_HANDLE;
    }
    return *this;
}

Texture::~Texture() { cleanup(); }

void Texture::init(const unsigned char* data, uint32_t width, uint32_t height) {
    // Create staging buffer
    m_stagingBuffer = std::make_unique<Buffer>(m_Device, "textureStagingBuffer", STBI_rgb_alpha, width * height, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_stagingBuffer->map();
    m_stagingBuffer->writeToBuffer(data);
    m_stagingBuffer->unmap();

    constexpr VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    createImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Prepare for buffer to image copy
    transitionImageLayout(m_textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(m_stagingBuffer->getBuffer(), m_textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    // Prepare for reading from shader
    transitionImageLayout(m_textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    createImageView(format);
    createTextureSampler();

    m_imageInfo = std::make_shared<VkDescriptorImageInfo>();
    m_imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    m_imageInfo->imageView = m_textureImageView;
    m_imageInfo->sampler = m_textureSampler;
}

void Texture::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                          VkMemoryPropertyFlags properties) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;  // Optional, interesting for sparse images

    if (vkCreateImage(m_Device.device(), &imageInfo, nullptr, &m_textureImage) != VK_SUCCESS) {
        RAYX_ERR << "Failed to create image";
    }

    // Allocate memory
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_Device.device(), m_textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Device.findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_Device.device(), &allocInfo, nullptr, &m_textureMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_Device.device(), m_textureImage, m_textureMemory, 0);
}

void Texture::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = m_Device.beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    // Do not transfer queue family ownership
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    // Define what needs to wait on the layout transition
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        // Do not wait on anything
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        // Do this during transfer stage (at the start of pipeline)
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // Wait on data transfer
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        // Do this before fragment shader reads
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer,                  //
                         sourceStage, destinationStage,  //
                         0,                              //
                         0, nullptr,                     //
                         0, nullptr,                     //
                         1, &barrier);

    m_Device.endSingleTimeCommands(commandBuffer);
}

void Texture::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = m_Device.beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    m_Device.endSingleTimeCommands(commandBuffer);
}

void Texture::createImageView(VkFormat format) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_Device.device(), &viewInfo, nullptr, &m_textureImageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
}

void Texture::createTextureSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_Device.physicalDevice(), &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(m_Device.device(), &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void Texture::cleanup() {
    vkDestroySampler(m_Device.device(), m_textureSampler, nullptr);
    vkDestroyImageView(m_Device.device(), m_textureImageView, nullptr);
    vkDestroyImage(m_Device.device(), m_textureImage, nullptr);
    vkFreeMemory(m_Device.device(), m_textureMemory, nullptr);

    // Reset handles to VK_NULL_HANDLE to mark them as no longer owned
    m_textureSampler = VK_NULL_HANDLE;
    m_textureImageView = VK_NULL_HANDLE;
    m_textureImage = VK_NULL_HANDLE;
    m_textureMemory = VK_NULL_HANDLE;
}