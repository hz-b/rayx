#ifndef NO_VULKAN
#pragma once
#include <vulkan/vulkan.hpp>

#include "RAY-Core.h"

namespace RAYX {
/**
 * @brief Specialized pushConsant Class
 *
 */
class RAYX_API PushConstantHandler {
  public:
    PushConstantHandler() = default;
    ~PushConstantHandler() = default;

    void update(void* data, uint32_t size);
    void* getData() const { return m_data; }
    uint32_t getSize() { return m_size; }
    VkPushConstantRange getVkPushConstantRange(VkShaderStageFlagBits flag, uint32_t offset = 0);

    template <class T>
    auto getActualPushConstant() {
        return reinterpret_cast<T*>(m_data);
    }

  private:
    // Pointer to struct containing push constant data
    void* m_data = nullptr;
    uint32_t m_size = 0;
};

}  // namespace RAYX
#endif