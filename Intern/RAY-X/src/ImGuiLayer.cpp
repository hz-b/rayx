#include "ImGuiLayer.h"

#include "Window.h"

ImGuiLayer::ImGuiLayer() : m_MainWindowData(), m_minImageCount(2), m_swapChainRebuild(false) {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::init() {}

void ImGuiLayer::setupContext(ImGui_ImplVulkan_InitInfo* init_info, GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends

    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_Init(init_info, m_MainWindowData.RenderPass);
}