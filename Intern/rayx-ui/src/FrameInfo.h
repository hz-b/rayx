#pragma once

#include <vulkan/vulkan.h>

#include "Camera.h"

struct FrameInfo {
    uint32_t frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    VkDescriptorSet descriptorSet;
};