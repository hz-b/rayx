#include "Window.h"

#include "utils.h"

Window::Window(/* args */) {}

Window::~Window() {}

void glfw_error_callback(int error, const char* description) { fprintf(stderr, "GLFW Error %d: %s\n", error, description); }

void Window::init(WindowProps props) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) exit(1);

    // Create window with Vulkan context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), NULL, NULL);
    if (!glfwVulkanSupported()) {
        printf("GLFW: Vulkan Not Supported\n");
        exit(1);
    }

    // glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
}

void Window::createSurface(VkInstance instance, VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) {
    VkResult err = glfwCreateWindowSurface(instance, m_Window, allocator, surface);
    check_vk_result(err);
}