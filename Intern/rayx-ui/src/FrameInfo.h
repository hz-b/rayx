#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "Camera.h"

struct FrameInfo {
    uint32_t frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    VkDescriptorSet descriptorSet;
    bool wasPathUpdated;
    std::string path;
};