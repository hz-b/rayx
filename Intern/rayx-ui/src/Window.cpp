#include "Window.h"

// TODO(Jannis): add state to check if window is initialized
Window::Window() {}

Window::~Window() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::initWindow(uint32_t width, uint32_t height, const char* title) {
    m_width = width;
    m_height = height;
    m_title = title;

    createWindow();
}

void Window::createWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->m_framebufferResized = true;
}