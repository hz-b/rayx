#include "Window.h"

#include <SDL3/SDL_vulkan.h>

#include <stdexcept>

Window::Window(uint32_t width, uint32_t height, const char* title) {
    m_width  = width;
    m_height = height;
    m_title  = title;

    SDL_SetHint(SDL_HINT_WINDOWS_GAMEINPUT, "0");
    SDL_Init(SDL_INIT_VIDEO);

    m_Window = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN);
    SDL_SetWindowResizable(m_Window, true);
}

Window::~Window() { SDL_Quit(); }

/**
 * Creates a Vulkan surface associated with the window.
 */
void Window::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (!SDL_Vulkan_CreateSurface(m_Window, instance, nullptr, surface)) { throw std::runtime_error("failed to create window surface!"); }
}
