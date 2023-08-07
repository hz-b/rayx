#ifndef NO_VULKAN
#pragma once
#include <vulkan/vulkan.hpp>

#include "Debug/Debug.h"

// Used for validating return values of Vulkan API calls.
inline void checkVkResult(VkResult result) {
    if (result != VK_SUCCESS) {
        RAYX_ERR << "Vulkan Error: " << result;
        throw std::runtime_error("Vulkan function call failed.");
    }
}
#endif