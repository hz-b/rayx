#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

struct [[maybe_unused]] Extent2D {
    uint32_t width;
    uint32_t height;
};

/**
 * @brief Class representing a window in RAYX-UI.
 *
 * This class is responsible for window creation, handling events and resizing,
 * as well as surface creation for Vulkan.
 *
 * This class is non-copyable.
 */
class Window {
  public:
    /**
     * @brief Construct a new Window object.
     * @param width The initial width of the window.
     * @param height The initial height of the window.
     * @param title The title of the window.
     */
    Window(uint32_t width, uint32_t height, const char* title);

    /// Deleted copy constructor.
    Window(const Window&) = delete;
    /// Deleted copy assignment operator.
    Window& operator=(const Window&) = delete;
    /// Destructor that destroys the window and terminates GLFW.
    ~Window();

    /**
     * @brief Creates a Vulkan surface associated with the window.
     * @param instance The Vulkan instance.
     * @param surface The Vulkan surface.
     * @throws std::runtime_error if the surface creation fails.
     */
    void createSurface(VkInstance instance, VkSurfaceKHR* surface);

    GLFWwindow* window() const { return m_Window; }
    VkExtent2D getExtent() const { return {m_width, m_height}; }

    bool shouldClose() const { return glfwWindowShouldClose(m_Window); }
    bool isMinimized() const { return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED); }

    bool wasWindowResized() { return m_framebufferResized; }
    void resetWindowResizedFlag() { m_framebufferResized = false; }

  private:
    /**
     * @brief Callback function for when the framebuffer is resized.
     * @param rawWindow The raw GLFW window.
     * @param width The new width of the framebuffer.
     * @param height The new height of the framebuffer.
     */
    static void framebufferResizeCallback(GLFWwindow* rawWindow, int width, int height);
    void onFramebufferResize(int width, int height);

    uint32_t m_width;
    uint32_t m_height;
    bool m_framebufferResized = false;
    const char* m_title;

    GLFWwindow* m_Window;
};
