#include "Texture.h"

#include "Debug/Debug.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <utility>

#include "Buffer.h"
#include "Debug/Instrumentor.h"
#include "Rml/Locate.h"

Texture::Texture(const Device& device)
    : m_Device{device},
      m_extent{0, 0},
      m_format{VK_FORMAT_R8G8B8A8_UNORM},
      m_layout{VK_IMAGE_LAYOUT_UNDEFINED},
      m_usageFlags{VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT},
      m_aspectFlags{VK_IMAGE_ASPECT_COLOR_BIT} {
    std::vector<uint8_t> data = dataFromPath(rayx::ResourceHandler::getInstance().getResourcePath("Textures/default.png"));
    createImage(VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    createImageView();
    createSampler();
    updateFromData(data.data(), m_extent.width, m_extent.height);
}

Texture::Texture(const Device& device, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectFlags, VkExtent2D extent)
    : m_Device{device},  //
      m_extent{extent},
      m_format{format},
      m_layout{VK_IMAGE_LAYOUT_UNDEFINED},
      m_usageFlags(usage),
      m_aspectFlags{aspectFlags} {
    RAYX_VERB << "Creating texture with format: " << format << " usage: " << usage << " aspect: " << aspectFlags;
    createImage(VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    createImageView();
    createSampler();
}

Texture::~Texture() {
    vkDeviceWaitIdle(m_Device.device());
    cleanup();
}

Texture::Texture(Texture&& other) noexcept
    : m_Device(other.m_Device),  // Assuming Device is a type that supports shallow copy or is a reference
      m_extent(other.m_extent),
      m_format(other.m_format),
      m_image(other.m_image),
      m_memory(other.m_memory),
      m_view(other.m_view),
      m_sampler(other.m_sampler),
      m_layout(other.m_layout) {
    // Invalidate the source object to prevent double deletion
    other.m_image   = VK_NULL_HANDLE;
    other.m_memory  = VK_NULL_HANDLE;
    other.m_view    = VK_NULL_HANDLE;
    other.m_sampler = VK_NULL_HANDLE;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {  // Prevent self-assignment
        // Clean up existing resources
        cleanup();

        // Copy data
        m_extent  = other.m_extent;
        m_format  = other.m_format;
        m_image   = other.m_image;
        m_memory  = other.m_memory;
        m_view    = other.m_view;
        m_sampler = other.m_sampler;
        m_layout  = other.m_layout;

        // Invalidate the source object
        other.m_image   = VK_NULL_HANDLE;
        other.m_memory  = VK_NULL_HANDLE;
        other.m_view    = VK_NULL_HANDLE;
        other.m_sampler = VK_NULL_HANDLE;
    }
    return *this;
}

void Texture::updateFromData(const unsigned char* data, uint32_t width, uint32_t height) {
    // RAYX_PROFILE_FUNCTION_STDOUT();
    m_extent.width  = width;
    m_extent.height = height;

    // Create staging buffer
    // TODO: parameterize required format of load (STBI_grey, STBI_grey_alpha, STBI_rgb, STBI_rgb_alpha, STBI_rgb_hdr,
    // STBI_rgba_hdr)
    Buffer buff(m_Device, "textureStagingBuffer", STBI_rgb_alpha, width * height, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    buff.map();
    buff.writeToBuffer(data);
    buff.unmap();

    // Copy data to image
    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(buff.getBuffer(), width, height);
    transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Texture::updateFromPath(const std::filesystem::path& path) {
    auto data = dataFromPath(path);
    updateFromData(data.data(), m_extent.width, m_extent.height);
}

std::vector<uint8_t> Texture::dataFromPath(const std::filesystem::path& path) {
    // TODO: parameterize required format of load
    int width, height, _;
    stbi_uc* pixels = stbi_load(path.string().c_str(), reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height), &_, STBI_rgb_alpha);
    if (!pixels) { RAYX_EXIT << "Failed to load texture image: " << path.string(); }
    m_extent.width  = static_cast<uint32_t>(width);
    m_extent.height = static_cast<uint32_t>(height);

    std::vector<uint8_t> data(pixels, pixels + width * height * 4);
    stbi_image_free(pixels);
    return data;
}

void Texture::resize(uint32_t width, uint32_t height) {
    m_extent.width  = width;
    m_extent.height = height;
    RAYX_VERB << "Resizing texture to " << width << "x" << height;
    RAYX_VERB << "Usage flags: " << m_usageFlags << " Aspect flags: " << m_aspectFlags << " Format: " << m_format;
    vkDeviceWaitIdle(m_Device.device());
    cleanup();
    createImage(VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    createImageView();
    createSampler();
}

void Texture::createImage(VkImageTiling tiling, VkMemoryPropertyFlags properties) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = m_extent.width;
    imageInfo.extent.height = m_extent.height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = m_format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = m_usageFlags;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags         = 0;  // Optional, interesting for sparse images

    if (vkCreateImage(m_Device.device(), &imageInfo, nullptr, &m_image) != VK_SUCCESS) { RAYX_EXIT << "Failed to create image"; }

    // Allocate memory
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_Device.device(), m_image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Device.findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_Device.device(), &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_Device.device(), m_image, m_memory, 0);
}

void Texture::transitionImageLayout(VkImageLayout newLayout) {
    if (m_layout == newLayout) { return; }
    VkCommandBuffer commandBuffer = m_Device.beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout           = m_layout;  // Use current layout
    barrier.newLayout           = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image               = m_image;

    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    // Setup aspect mask and determine src/dst access masks and stages
    VkPipelineStageFlags sourceStage, destinationStage;
    setupTransitionDetails(m_layout, newLayout, barrier.subresourceRange.aspectMask, barrier.srcAccessMask, barrier.dstAccessMask, sourceStage,
                           destinationStage);

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    m_Device.endSingleTimeCommands(commandBuffer);
    m_layout = newLayout;  // Update current layout
}

void Texture::setupTransitionDetails(VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags& aspectMask, VkAccessFlags& srcAccessMask,
                                     VkAccessFlags& dstAccessMask, VkPipelineStageFlags& sourceStage, VkPipelineStageFlags& destinationStage) {
    aspectMask = determineAspectMask();  // Implement based on image format

    // Define transitions
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        srcAccessMask    = 0;
        dstAccessMask    = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        srcAccessMask    = 0;
        dstAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        srcAccessMask    = 0;
        dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        srcAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstAccessMask    = VK_ACCESS_SHADER_READ_BIT;
        sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        srcAccessMask    = 0;
        dstAccessMask    = VK_ACCESS_SHADER_READ_BIT;
        sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dstAccessMask    = VK_ACCESS_SHADER_READ_BIT;
        sourceStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        srcAccessMask    = VK_ACCESS_SHADER_READ_BIT;
        dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        sourceStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else {
        throw std::invalid_argument("Unsupported layout transition!");
    }
}

VkImageAspectFlags Texture::determineAspectMask() const {
    if (m_format == VK_FORMAT_D32_SFLOAT || m_format == VK_FORMAT_D24_UNORM_S8_UINT) {
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    } else {
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

void Texture::copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = m_Device.beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset      = 0;
    region.bufferRowLength   = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask     = m_aspectFlags;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    m_Device.endSingleTimeCommands(commandBuffer);
}

void Texture::createImageView() {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = m_image;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = m_format;
    viewInfo.subresourceRange.aspectMask     = m_aspectFlags;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(m_Device.device(), &viewInfo, nullptr, &m_view) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
}

void Texture::createSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter        = VK_FILTER_NEAREST;
    samplerInfo.minFilter        = VK_FILTER_NEAREST;
    samplerInfo.addressModeU     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_Device.physicalDevice(), &properties);

    samplerInfo.maxAnisotropy           = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable           = VK_FALSE;
    samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias              = 0.0f;
    samplerInfo.minLod                  = 0.0f;
    samplerInfo.maxLod                  = 0.0f;

    if (vkCreateSampler(m_Device.device(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void Texture::cleanup() {
    vkDestroySampler(m_Device.device(), m_sampler, nullptr);
    vkDestroyImageView(m_Device.device(), m_view, nullptr);
    vkDestroyImage(m_Device.device(), m_image, nullptr);
    vkFreeMemory(m_Device.device(), m_memory, nullptr);

    // Reset handles to VK_NULL_HANDLE to mark them as no longer owned
    m_sampler = VK_NULL_HANDLE;
    m_view    = VK_NULL_HANDLE;
    m_image   = VK_NULL_HANDLE;
    m_memory  = VK_NULL_HANDLE;
}
