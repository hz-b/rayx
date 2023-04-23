#include "Window.h"

Window::Window(GLFWerrorfun cbfun)
{
    glfwSetErrorCallback(cbfun);
    if (!glfwInit())
        exit(1);
    // Create window with Vulkan context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_Window = glfwCreateWindow(1920, 1080, "Dear ImGui GLFW+Vulkan example", NULL, NULL);
    if (!glfwVulkanSupported())
    {
        printf("GLFW: Vulkan Not Supported\n");
        exit(1);
    }
}

// TODO(Jannis): Vulkan uses m_Window -> maybe life time issues
Window::~Window()
{
    free(m_Window);
}

