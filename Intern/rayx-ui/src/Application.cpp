#include "Application.h"

#include <chrono>

#include "Data/Importer.h"
#include "FrameInfo.h"
#include "GraphicsCore/Renderer.h"
#include "GraphicsCore/Window.h"
#include "RenderObject.h"
#include "RenderSystem/ObjectRenderSystem.h"
#include "RenderSystem/RayRenderSystem.h"
#include "RenderSystem/UIRenderSystem.h"
#include "Triangulation/Triangulate.h"
#include "UserInput.h"
#include "Writer/CSVWriter.h"
#include "Writer/H5Writer.h"

// --------- Start of Application code --------- //
Application::Application(uint32_t width, uint32_t height, const char* name, int argc, char** argv)
    : m_Window(width, height, name),   //
      m_Device(m_Window),              //
      m_Renderer(m_Window, m_Device),  //
      m_CommandParser(argc, argv)      //
{
    m_DescriptorPool = DescriptorPool::Builder(m_Device)
                           .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                           .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                           .build();
}

Application::~Application() = default;

void Application::run() {
    m_CommandParser.analyzeCommands();

    // Create UBOs (Uniform Buffer Object)
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (auto& uboBuffer : uboBuffers) {
        uboBuffer = std::make_unique<Buffer>(m_Device, "uboBuffer", sizeof(Camera), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffer->map();
    }

    // Descriptor set layout
    auto setLayout = DescriptorSetLayout::Builder(m_Device)
                         .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  //
                         .build();                                                                      //
    std::vector<VkDescriptorSet> descriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (unsigned long i = 0; i < descriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*setLayout, *m_DescriptorPool).writeBuffer(0, &bufferInfo).build(descriptorSets[i]);
    }

    // Render systems
    ObjectRenderSystem objectRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass(), setLayout->getDescriptorSetLayout());
    RayRenderSystem rayRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass(), setLayout->getDescriptorSetLayout());
    UIRenderSystem m_ImGuiLayer(m_Window, m_Device, m_Renderer.getSwapChainImageFormat(), m_Renderer.getSwapChainDepthFormat(),
                                m_Renderer.getSwapChainImageCount());

    // Camera
    CameraController camController;
    Camera cam;

    // Input callbacks
    glfwSetKeyCallback(m_Window.window(), keyCallback);
    glfwSetMouseButtonCallback(m_Window.window(), mouseButtonCallback);
    glfwSetCursorPosCallback(m_Window.window(), cursorPosCallback);
    glfwSetWindowUserPointer(m_Window.window(), &camController);

    auto currentTime = std::chrono::high_resolution_clock::now();
    std::vector<RenderObject> rObjects;
    std::vector<Line> rays;
    std::optional<RenderObject> rayObj;

    // CLI Input
    std::string rmlPathCli = m_CommandParser.m_args.m_providedFile;
    if (!rmlPathCli.empty()) {
        updateScene(rmlPathCli.c_str(), rObjects, rays, rayObj);
    }

    // Main loop
    while (!m_Window.shouldClose()) {
        glfwPollEvents();

        // Skip rendering when minimized
        if (m_Window.isMinimized()) {
            continue;
        }

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        if (auto commandBuffer = m_Renderer.beginFrame()) {
            uint32_t frameIndex = m_Renderer.getFrameIndex();

            FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, cam, descriptorSets[frameIndex], false, ""};
            m_ImGuiLayer.setupUI(camController, frameInfo);
            if (frameInfo.wasPathUpdated) {
                updateScene(frameInfo.filePath.c_str(), rObjects, rays, rayObj);
                frameInfo.wasPathUpdated = false;
                camController.lookAtPoint(rObjects[0].getTranslationVecor());
            }

            camController.update(cam, m_Renderer.getAspectRatio());

            // Update UBO
            uboBuffers[frameIndex]->writeToBuffer(&cam);
            uboBuffers[frameIndex]->flush();

            // Render
            m_Renderer.beginSwapChainRenderPass(commandBuffer, m_ImGuiLayer.getClearValue());

            objectRenderSystem.render(frameInfo, rObjects);
            rayRenderSystem.render(frameInfo, rayObj);
            m_ImGuiLayer.render(commandBuffer);

            m_Renderer.endSwapChainRenderPass(commandBuffer);
            m_Renderer.endFrame();
        }
    }
    vkDeviceWaitIdle(m_Device.device());
}

void Application::updateScene(const std::string& path, std::vector<RenderObject>& rObjects, std::vector<Line>& rays,
                              std::optional<RenderObject>& rayObj) {
    RAYX::Beamline beamline = RAYX::importBeamline(path);
#ifndef NO_H5
    std::string rayFilePath = path.substr(0, path.size() - 4) + ".h5";
    RAYX::BundleHistory bundleHist = raysFromH5(rayFilePath, FULL_FORMAT);
#else
    std::string rayFilePath = path.substr(0, path.size() - 4) + ".csv";
    RAYX::BundleHistory bundleHist = loadCSV(rayFilePath);
#endif
    vkDeviceWaitIdle(m_Device.device());  // TODO(Jannis): Hacky fix for now; should be some form of synchronization

    // Triangulate the render data and update the scene
    rObjects = triangulateObjects(beamline.m_OpticalElements, m_Device, false);
    rays = getRays(bundleHist, beamline.m_OpticalElements);

    if (!rays.empty()) {
        // Temporarily aggregate all vertices, then create a single RenderObject
        std::vector<Vertex> rayVertices(rays.size() * 2);
        std::vector<uint32_t> rayIndices(rays.size() * 2);
        for (uint32_t i = 0; i < rays.size(); ++i) {
            rayVertices[i * 2] = rays[i].v1;
            rayVertices[i * 2 + 1] = rays[i].v2;
            rayIndices[i * 2] = i * 2;
            rayIndices[i * 2 + 1] = i * 2 + 1;
        }
        rayObj.emplace(m_Device, glm::mat4(1.0f), rayVertices, rayIndices);
    }
}