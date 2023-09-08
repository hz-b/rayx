#include "Application.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>

#include "Data/Importer.h"
#include "FrameInfo.h"
#include "RenderObject.h"
#include "RenderSystem/LineRenderSystem.h"
#include "RenderSystem/TriangleRenderSystem.h"
#include "Writer/H5Writer.h"

// Key callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto camController = reinterpret_cast<CameraController*>(glfwGetWindowUserPointer(window));

    // The movement speed factor
    float speed = (mods & GLFW_MOD_SHIFT) ? 20.0f : 0.5f;

    // Quit
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    // Fullscreen
    else if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    // Windowed
    else if (key == GLFW_KEY_F10 && action == GLFW_PRESS) {
        glfwSetWindowMonitor(window, nullptr, 100, 100, 1280, 720, 60);
    }
    // wasd movement (+ faster movement with shift)
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveForward(speed);
    } else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveForward(-speed);
    } else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveSideways(-speed);
    } else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveSideways(speed);
    } else if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveUp(-speed);
    } else if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveUp(speed);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto camController = reinterpret_cast<CameraController*>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            camController->startMouseLook();
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            camController->setLastMousePos(mouseX, mouseY);
        } else if (action == GLFW_RELEASE) {
            camController->stopMouseLook();
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto camController = reinterpret_cast<CameraController*>(glfwGetWindowUserPointer(window));

    if (camController->isMouseLooking()) {
        camController->updateDirectionViaMouse(xpos, ypos);
    }
}

// --------- Start of Application code --------- //
Application::Application(uint32_t width, uint32_t height, const char* name)
    : m_Window(width, height, name),  //
      m_Device(m_Window),             //
      m_Renderer(m_Window, m_Device),
      // m_ImGuiLayer(m_Window, m_Device),  //
      m_Scene(m_Device) {
    // Building the descriptor pool
    m_DescriptorPool = DescriptorPool::Builder(m_Device)
                           .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                           .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                           .build();
}

Application::~Application() {}

void Application::run() {
    // Get the render data
    std::string path = "MultiRZP_101_0.00203483_groupedCCD";
    std::vector<RenderObject> vec = RenderObject::getRenderData(std::string(path + ".rml"));
    RAYX::BundleHistory rays;
    readH5(rays, std::string(path + ".h5"), FULL_FORMAT);
    m_Scene.update(vec, rays);

    glfwSetKeyCallback(m_Window.window(), keyCallback);
    glfwSetMouseButtonCallback(m_Window.window(), mouseButtonCallback);
    glfwSetCursorPosCallback(m_Window.window(), cursorPosCallback);

    // UBOs
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] =
            std::make_unique<Buffer>(m_Device, sizeof(Camera), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    // Descriptor set layout
    auto setLayout = DescriptorSetLayout::Builder(m_Device)
                         .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  //
                         .build();                                                                      //

    std::vector<VkDescriptorSet> descriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < descriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*setLayout, *m_DescriptorPool).writeBuffer(0, &bufferInfo).build(descriptorSets[i]);
    }

    TriangleRenderSystem triangleRenderSystem(m_Device, m_Scene, m_Renderer.getSwapChainRenderPass(), setLayout->getDescriptorSetLayout());
    LineRenderSystem lineRenderSystem(m_Device, m_Scene, m_Renderer.getSwapChainRenderPass(), setLayout->getDescriptorSetLayout());

    CameraController camController;
    Camera cam;

    glfwSetWindowUserPointer(m_Window.window(), &camController);

    auto currentTime = std::chrono::high_resolution_clock::now();
    while (!m_Window.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;
        // TODO: ImGui layer should not be in renderer class (maybe its own render system)
        m_Renderer.updateImGui(camController, frameTime);

        if (auto commandBuffer = m_Renderer.beginFrame()) {
            uint32_t frameIndex = m_Renderer.getFrameIndex();
            camController.update(cam, m_Renderer.getAspectRatio());
            FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, cam, descriptorSets[frameIndex]};

            // Update ubo
            uboBuffers[frameIndex]->writeToBuffer(&cam);
            uboBuffers[frameIndex]->flush();

            // render
            m_Renderer.beginSwapChainRenderPass(commandBuffer);

            triangleRenderSystem.render(frameInfo);
            lineRenderSystem.render(frameInfo);

            m_Renderer.endSwapChainRenderPass(commandBuffer);
            m_Renderer.endFrame();
        }
    }
    vkDeviceWaitIdle(m_Device.device());
}
