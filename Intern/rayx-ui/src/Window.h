#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Extent2D {
    uint32_t width;
    uint32_t height;
};

class Window {
  public:
    Window(uint32_t width, uint32_t height, const char* const title);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    ~Window();

    GLFWwindow* get() const { return m_Window; }
    Extent2D getExtent() const { return {m_width, m_height}; }
    void updateWindowSize();
    bool shouldClose() const { return glfwWindowShouldClose(m_Window); }

    bool m_framebufferResized = false;

  private:
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    uint32_t m_width;
    uint32_t m_height;
    const char* m_title;

    GLFWwindow* m_Window;
    VkSurfaceKHR m_Surface;
};
