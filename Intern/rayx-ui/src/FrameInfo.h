#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "Camera.h"

struct FrameInfo {
    Camera& camera;
    uint32_t frameIndex;
    VkCommandBuffer commandBuffer;
    VkDescriptorSet descriptorSet;
};