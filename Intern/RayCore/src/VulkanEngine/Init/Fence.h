#ifndef NO_VULKAN
#include "vulkan/vulkan.hpp"
namespace RAYX {
class NewFence {
  public:
    NewFence(VkDevice& device);
    ~NewFence();
    VkFence* fence();
    VkResult wait();
    VkResult forceReset();

  private:
    VkFence f;
    VkDevice device;
};
}  // namespace RAYX

#endif