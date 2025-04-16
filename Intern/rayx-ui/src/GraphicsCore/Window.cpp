#include "Window.h"

#include <SDL3/SDL_vulkan.h>

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

    SDL_SetHint(SDL_HINT_WINDOWS_GAMEINPUT, "0");
    SDL_Init(SDL_INIT_VIDEO);

    m_Window = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN);
    SDL_SetWindowResizable(m_Window, true);
    // glfwSetWindowUserPointer(m_Window, this);
    // glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
}

Window::~Window() { SDL_Quit(); }

/**
 * Callback function for when the framebuffer is resized. Sets the framebufferResized flag to true.
 * Resizes the window to the new framebuffer size.
 */
void Window::framebufferResizeCallback(SDL_Window* rawWindow, int width, int height) {
    auto window = reinterpret_cast<Window*>(rawWindow);  // TODO: This...
    if (window) {
        window->onFramebufferResize(width, height);  // TODO: and this is pretty weird
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
    if (!SDL_Vulkan_CreateSurface(m_Window, instance, nullptr, surface)) {
        throw std::runtime_error("failed to create window surface!");
    }
}
