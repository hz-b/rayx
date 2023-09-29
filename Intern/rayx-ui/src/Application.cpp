#include "Application.h"

#include <chrono>

#include "Data/Importer.h"
#include "FrameInfo.h"
#include "GraphicsCore/Renderer.h"
#include "GraphicsCore/Window.h"
#include "RenderObject.h"
#include "RenderSystem/ObjectRenderSystem.h"
#include "RenderSystem/RayRenderSystem.h"
#include "Triangulation/Triangulate.h"
#include "UserInput.h"
#include "Writer/CSVWriter.h"
#include "Writer/H5Writer.h"

// --------- Start of Application code --------- //
Application::Application(uint32_t width, uint32_t height, const char* name)
    : m_Window(width, height, name),  //
      m_Device(m_Window),             //
      m_Renderer(m_Window, m_Device)  //
{
    m_DescriptorPool = DescriptorPool::Builder(m_Device)
                           .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                           .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                           .build();
}

Application::~Application() {}

void Application::run() {
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

    // Render systems
    ObjectRenderSystem objectRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass(), setLayout->getDescriptorSetLayout());
    RayRenderSystem rayRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass(), setLayout->getDescriptorSetLayout());

    // Camera
    CameraController camController;
    Camera cam;

    // Input callbacks
    glfwSetKeyCallback(m_Window.window(), keyCallback);
    glfwSetMouseButtonCallback(m_Window.window(), mouseButtonCallback);
    glfwSetCursorPosCallback(m_Window.window(), cursorPosCallback);
    glfwSetWindowUserPointer(m_Window.window(), &camController);

    // Main loop
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::vector<RenderObject> rObjects;
    std::vector<Line> rays;

    // Temporary triangle to render
    std::vector<Vertex> vertices = {
        {{0.0f, -0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},  //
        {{0.5f, 0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},   //
        {{-0.5f, 0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}   //
    };
    std::vector<uint32_t> indices = {0, 1, 2};
    rObjects.emplace_back(std::move(RenderObject(m_Device, glm::mat4(1.0f), vertices, indices)));
    // Add another one
    vertices[0].pos = {0.5f, -0.5f, 0.5f, 1.0f};
    rObjects.emplace_back(std::move(RenderObject(m_Device, glm::mat4(1.0f), vertices, indices)));
    // Add another one
    vertices[0].pos = {-0.5f, -0.5f, 0.5f, 1.0f};
    rObjects.emplace_back(std::move(RenderObject(m_Device, glm::mat4(1.0f), vertices, indices)));

    // Temporary ray to render
    Line ray = {
        {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},  //
        {{0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}   //
    };
    rays.emplace_back(ray);

    while (!m_Window.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        if (auto commandBuffer = m_Renderer.beginFrame()) {
            uint32_t frameIndex = m_Renderer.getFrameIndex();
            camController.update(cam, m_Renderer.getAspectRatio());
            FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, cam, descriptorSets[frameIndex]};
            // TODO: ImGui layer should not be in renderer class (maybe its own render system)
            m_Renderer.updateImGui(camController, frameInfo);

            if (frameInfo.wasPathUpdated) {
                // Get the render data
                RAYX::Beamline beamline = RAYX::importBeamline(frameInfo.rmlPath);
#ifndef NO_H5
                RAYX::BundleHistory bundleHist = raysFromH5(frameInfo.rayFilePath, FULL_FORMAT);
#else
                RAYX::BundleHistory bundleHist = loadCSV(frameInfo.rayFilePath);
#endif

                // Triangulate the render data and update the scene
                rObjects = triangulateObjects(beamline.m_OpticalElements, m_Device, true);
                rays = getRays(bundleHist, beamline.m_OpticalElements);
            }

            // Update ubo
            uboBuffers[frameIndex]->writeToBuffer(&cam);
            uboBuffers[frameIndex]->flush();

            // Render
            m_Renderer.beginSwapChainRenderPass(commandBuffer);

            objectRenderSystem.render(frameInfo, rObjects);
            rayRenderSystem.render(frameInfo, rays);

            m_Renderer.endSwapChainRenderPass(commandBuffer);
            m_Renderer.endFrame();
        }
    }
    vkDeviceWaitIdle(m_Device.device());
}
