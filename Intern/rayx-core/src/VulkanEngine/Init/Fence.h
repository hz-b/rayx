#ifndef NO_VULKAN
#pragma once
#include "vulkan/vulkan.hpp"
namespace RAYX {
// TODO : Rename this to Fence, once the older one is removed

/**
 * @brief Synchronization vulkan object 
 * 
 */
class Fence {
  public:
    Fence(VkDevice& device);
    ~Fence();
    VkFence* fence();
    VkResult waitAndReset();
    VkResult forceReset();

  private:
    VkFence f;
    VkDevice device;
};
}  // namespace RAYX

#endif