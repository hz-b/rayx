#ifndef NO_VULKAN
#pragma once

#define IS_ENGINE_PREINIT                                    \
    if (m_state != EngineStates_t::PREINIT) {                \
        RAYX_ERR << "VulkanEngine was already initialized!"; \
    }

#define IS_ENGINE_POSTRUN                                                 \
    if (m_state != EngineStates_t::POSTRUN) {                             \
        RAYX_ERR << "you've forgotton to .run() the VulkanEngine. Thats " \
                    "mandatory before reading it's output buffers.";      \
    }

#define IS_ENGINE_CLEAN                                                \
    if (m_state == EngineStates_t::PREINIT) {                          \
        RAYX_ERR << "you've forgotten to .init() the VulkanEngine";    \
    } else if (m_state == EngineStates_t::POSTRUN) {                   \
        RAYX_ERR << "you've forgotten to .cleanup() the VulkanEngine"; \
    }

#define IS_ENGINE_CLEANABLE                                            \
    if (m_state != EngineStates_t::POSTRUN) {                          \
        RAYX_ERR << "cleanup() only needs to be called after .run()!"; \
    }

// Used for validating return values of Vulkan API calls.
#define VK_CHECK_RESULT(f)                                               \
    {                                                                    \
        VkResult res = (f);                                              \
        if (res != VK_SUCCESS) {                                         \
            RAYX_WARN << "Fatal : VkResult fail!";                       \
            RAYX_ERR << "Error code: " << res                            \
                     << ", look up at "                                  \
                        "https://www.khronos.org/registry/vulkan/specs/" \
                        "1.3-extensions/man/html/VkResult.html";         \
        }                                                                \
    }

#endif;