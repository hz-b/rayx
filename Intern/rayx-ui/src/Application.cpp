#include "Application.h"

#include <chrono>

#include "Colors.h"
#include "Data/Importer.h"
#include "Debug/Debug.h"
#include "FrameInfo.h"
#include "GraphicsCore/Renderer.h"
#include "GraphicsCore/Window.h"
#include "RenderObject.h"
#include "RenderSystem/GridRenderSystem.h"
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
    UIRenderSystem uiRenderSystem(m_Window, m_Device, m_Renderer.getSwapChainImageFormat(), m_Renderer.getSwapChainDepthFormat(),
                                  m_Renderer.getSwapChainImageCount());
    GridRenderSystem gridRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass(), setLayout->getDescriptorSetLayout());

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
    UIParameters uiParams{camController, rmlPathCli, !rmlPathCli.empty(), 0.0};

    // Main loop
    while (!m_Window.shouldClose()) {
        // Skip rendering when minimized
        if (m_Window.isMinimized()) {
            continue;
        }
        glfwPollEvents();

        if (auto commandBuffer = m_Renderer.beginFrame()) {
            // Params to pass to UI
            auto newTime = std::chrono::high_resolution_clock::now();
            uiParams.frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            // Update UI and camera
            uiRenderSystem.setupUI(uiParams, rObjects);
            // camController.update(cam, m_Renderer.getAspectRatio());
            if (uiParams.pathChanged) {
                updateScene(uiParams.rmlPath.string(), rObjects, rays, rayObj);
                uiParams.pathChanged = false;
                camController.lookAtPoint(rObjects[0].getTranslationVecor());
            }

            camController.update(cam, m_Renderer.getAspectRatio());

            // Update UBO
            uint32_t frameIndex = m_Renderer.getFrameIndex();
            uboBuffers[frameIndex]->writeToBuffer(&cam);
            // uboBuffers[frameIndex]->flush();

            // Render
            m_Renderer.beginSwapChainRenderPass(commandBuffer, uiRenderSystem.getClearValue());

            FrameInfo frameInfo{cam, frameIndex, commandBuffer, descriptorSets[frameIndex]};
            objectRenderSystem.render(frameInfo, rObjects);
            rayRenderSystem.render(frameInfo, rayObj);
            gridRenderSystem.render(frameInfo);
            uiRenderSystem.render(commandBuffer);

            m_Renderer.endSwapChainRenderPass(commandBuffer);
            m_Renderer.endFrame();
        } else {
            RAYX_LOG << "Failed to acquire swap chain image";
            break;
        }
    }
    vkDeviceWaitIdle(m_Device.device());
}

/**
 * This function processes the BundleHistory and determines the ray's path in the beamline.
 * Depending on the event type associated with the ray, the function produces visual lines that represent
 * ray segments, colored based on the event type.
 */
std::vector<Line> getRays(const RAYX::BundleHistory& bundleHist, const std::vector<RAYX::OpticalElement>& elements) {
    std::vector<Line> rays;

    for (const RAYX::RayHistory& rayHist : bundleHist) {
        glm::vec3 rayLastPos = {0.0f, 0.0f, 0.0f};
        for (const RAYX::Event& event : rayHist) {
            if (event.m_eventType == ETYPE_JUST_HIT_ELEM || event.m_eventType == ETYPE_ABSORBED) {
                // Events where rays hit objects are in element coordinates
                // We need to convert them to world coordinates
                glm::vec4 worldPos = elements[(size_t)event.m_lastElement].m_element.m_outTrans * glm::vec4(event.m_position, 1.0f);

                Vertex origin = {{rayLastPos.x, rayLastPos.y, rayLastPos.z, 1.0f}, YELLOW};
                Vertex point = (event.m_eventType == ETYPE_JUST_HIT_ELEM) ? Vertex(worldPos, ORANGE) : Vertex(worldPos, RED);

                Line myline = {origin, point};
                rays.push_back(myline);
                rayLastPos = point.pos;
            } else if (event.m_eventType == ETYPE_FLY_OFF) {
                // Fly off events are in world coordinates
                // The origin here is the position of the event
                // The point is defined by the direction of the ray (default length)

                glm::vec4 eventPos = glm::vec4(event.m_position, 1.0f);
                glm::vec4 eventDir = glm::vec4(event.m_direction, 0.0f);
                glm::vec4 pointPos = eventPos + eventDir * 1000.0f;

                Vertex origin = {eventPos, GREY};
                Vertex point = {pointPos, GREY};

                rays.push_back(Line(origin, point));
            } else if (event.m_eventType == ETYPE_NOT_ENOUGH_BOUNCES) {
                // Events where rays hit objects are in element coordinates
                // We need to convert them to world coordinates
                glm::vec4 worldPos = elements[(size_t)event.m_lastElement].m_element.m_outTrans * glm::vec4(event.m_position, 1.0f);

                const glm::vec4 white = {1.0f, 1.0f, 1.0f, 0.7f};
                Vertex origin = {{rayLastPos.x, rayLastPos.y, rayLastPos.z, 1.0f}, white};
                Vertex point = Vertex(worldPos, white);

                rays.push_back(Line(origin, point));
                rayLastPos = point.pos;
            }
        }
    }

    return rays;
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
    rObjects = triangulateObjects(beamline.m_OpticalElements, m_Device);
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
        rayObj.emplace("Rays", m_Device, glm::mat4(1.0f), rayVertices, rayIndices);
    }
}
