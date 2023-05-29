#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Extent2D {
    uint32_t width;
    uint32_t height;
};

class Window {
  public:
    Window();
    ~Window();

    void initWindow(uint32_t width, uint32_t height, const char* const title);

    GLFWwindow* get() const { return m_Window; }
    Extent2D getExtent() const { return {m_width, m_height}; }
    void updateWindowSize();
    
    bool m_framebufferResized = false;

  private:
    void createWindow();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    const GLFWwindow* window;
    uint32_t m_width;
    uint32_t m_height;
    const char* m_title;

    GLFWwindow* m_Window;
    VkSurfaceKHR m_Surface;

};
