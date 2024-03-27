#pragma once

#include <vulkan/vulkan.h>

// this RAII wrapper only works if vulkan uses 64bit pointers internally
static_assert(VK_USE_64_BIT_PTR_DEFINES);

template <typename T>
class RefDeleter {
  public:
    RefDeleter(VkDevice device, VkAllocationCallbacks* allocator) : m_device(device), m_allocator(allocator) {}

    inline void operator()(const T handle) {
        // sanity check: memory must not be a null handle
        assert(handle != VK_NULL_HANDLE);

        destroyResource(handle);
    }

  private:
    VkDevice m_device;
    VkAllocationCallbacks* m_allocator;

    inline void destroyResource(VkDeviceMemory handle) { vkFreeMemory(m_device, handle, m_allocator); }
    inline void destroyResource(VkImage handle) { vkDestroyImage(m_device, handle, m_allocator); }
    inline void destroyResource(VkImageView handle) { vkDestroyImageView(m_device, handle, m_allocator); }
    inline void destroyResource(VkSampler handle) { vkDestroySampler(device, handle, m_allocator); }
    inline void destroyResource(VkBuffer handle) { vkDestroyBuffer(device, handle, m_allocator); }
    inline void destroyResource(VkFramebuffer handle) { vkDestroyFramebuffer(device, handle, m_allocator); }
    inline void destroyResource(VkSemaphore handle) { vkDestroySemaphore(device, handle, m_allocator); }
    inline void destroyResource(VkFence handle) { vkDestroyFence(device, handle, m_allocator); }
};

template <typename T>
class Ref {
  public:
    Ref() : m_memory(static_cast<T>(VK_NULL_HANDLE)) {}
    Ref(VkDevice device, T handle, VkAllocationCallbacks* allocator = nullptr) : m_memory(handle, RefDeleter<T>(device, allocator)) {}

    Ref(const Ref& other) = default;
    Ref(Ref&& other) : Ref() { std::swap(m_memory, other.m_memory); }

    inline Ref& operator=(const Ref&) = default;
    inline Ref& operator=(Ref&& other) {
        std::swap(m_memory, other.m_memory);
        return *this;
    }

    // get raw handle to contained object
    inline T get() { return m_memory.get(); }
    inline T operator*() { return m_memory.get(); }

    // destroy contained object
    inline void reset() { m_memory.reset(); }

  private:
    std::shared_ptr<std::remove_pointer_t<T>> m_handle;
};
