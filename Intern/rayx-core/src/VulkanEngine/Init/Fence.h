#ifndef NO_VULKAN
#pragma once
#include "vulkan/vulkan.hpp"
namespace RAYX {
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