#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>

class Window
{
public:
    Window(GLFWerrorfun cbfun);
    ~Window();

    GLFWwindow* m_Window;

private:
    void glfwSetErrorCallback(GLFWerrorfun cbfun);
};
