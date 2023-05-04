#ifndef NO_VULKAN
#include "vulkan/vulkan.hpp"
namespace RAYX {
class Fence {
  public:
    Fence(VkDevice& device);
    ~Fence();
    VkFence* fence();
    VkResult wait();
    VkResult forceReset();

  private:
    VkFence f;
    VkDevice device;
};
}  // namespace RAYX

#endif