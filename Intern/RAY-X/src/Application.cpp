#include "Application.h"

#include "utils.h"

Application::Application() {
    m_Window.init(WindowProps("RAY-X", 1920, 1080));
    m_Renderer.init();
    m_ImGuiLayer.init();
}

Application::~Application() {}

void Application::run() {
    // while (!glfwWindowShouldClose(m_Window)) {
    //     glfwPollEvents();
    //     drawFrame();
    // }
    // vkDeviceWaitIdle(m_Device);
}

// TODO(Jannis): Where to put this? Maybe it should stay in the application class...
void Application::createSurface() {
    VkResult err = glfwCreateWindowSurface(m_Renderer.m_Instance, m_Window.m_Window, m_Renderer.m_Allocator, &m_Surface);
    check_vk_result(err);
}

void Application::createSwapChain() {}

void Application::createImageViews() {}

void Application::createRenderPass() {}

void Application::createGraphicsPipeline() {}

void Application::createCommandPool() {}

void Application::createCommandBuffers() {}

void Application::createSyncObjects() {}

void Application::drawFrame() {}
