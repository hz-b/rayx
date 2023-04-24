#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

struct WindowProps {
    std::string Title;
    uint32_t Width;
    uint32_t Height;

    WindowProps(const std::string& title = "Hazel Engine", uint32_t width = 1600, uint32_t height = 900)
        : Title(title), Width(width), Height(height) {}
};

class Window {
  public:
    Window(/* args */);
    ~Window();

    void init(WindowProps props);
    void createSurface(VkInstance instance, VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);

    GLFWwindow* m_Window;

  private:
    WindowProps m_Props;
};
