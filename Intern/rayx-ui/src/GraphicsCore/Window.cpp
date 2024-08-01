#include "Window.h"

#include <stdexcept>

#include "Debug/Debug.h"

/**
 * This constructor performs several key tasks:
 * - Initializes GLFW using glfwInit().
 * - Sets GLFW to not create an OpenGL context with glfwWindowHint().
 * - Creates a new GLFW window and stores its pointer.
 * - Sets the user-defined pointer of the GLFW window to this `Window` instance.
 * - Registers a framebuffer size callback function.
 *
 * @see: framebufferResizeCallback() for more information on the callback function.
 */
Window::Window(uint32_t width, uint32_t height, const char* title) {
    m_width = width;
    m_height = height;
    m_title = title;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
}

Window::~Window() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

/**
 * Callback function for when the framebuffer is resized. Sets the framebufferResized flag to true.
 * Resizes the window to the new framebuffer size.
 */
void Window::framebufferResizeCallback(GLFWwindow* rawWindow, int width, int height) {
    auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(rawWindow));
    if (window) {
        window->onFramebufferResize(width, height);
    } else {
        RAYX_EXIT << "Failed to get window pointer in framebuffer resize callback!";
    }
}

void Window::onFramebufferResize(int width, int height) {
    m_framebufferResized = true;
    m_width = width;
    m_height = height;
}

/**
 * Creates a Vulkan surface associated with the window.
 */
void Window::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}
