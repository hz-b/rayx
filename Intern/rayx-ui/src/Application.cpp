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
#include "UserInput.h"
#include "Writer/H5Writer.h"

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
    std::vector<RenderObject> vec = RenderObject::createRenderObjects(std::string(path + ".rml"));
    RAYX::BundleHistory rays;
    readH5(rays, std::string(path + ".h5"), FULL_FORMAT);
    m_Scene.update(vec, rays);

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

    glfwSetKeyCallback(m_Window.window(), keyCallback);
    glfwSetMouseButtonCallback(m_Window.window(), mouseButtonCallback);
    glfwSetCursorPosCallback(m_Window.window(), cursorPosCallback);
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
