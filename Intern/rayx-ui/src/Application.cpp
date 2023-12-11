#include "Application.h"

#include <chrono>
#include <unordered_set>

#include "CanonicalizePath.h"
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
#include "UserInput.h"
#include "Writer/CSVWriter.h"
#include "Writer/H5Writer.h"

// --------- Start of Application code --------- //
Application::Application(uint32_t width, uint32_t height, const char* name, int argc, char** argv)
    : m_Window(width, height, name),                          //
      m_CommandParser(argc, argv),                            //
      m_Device(m_Window, m_CommandParser.m_args.m_deviceID),  //
      m_Renderer(m_Window, m_Device)                          //
{
    m_DescriptorPool = DescriptorPool::Builder(m_Device)
                           .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                           .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
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
    BundleHistory rayCache;
    std::optional<RenderObject> rayObj;

    // CLI Input
    std::string rmlPathCli = m_CommandParser.m_args.m_providedFile;
    UIRayInfo rayInfo{true, false, false, false, 50, 100};
    UIParameters uiParams{camController, rmlPathCli, !rmlPathCli.empty(), 0.0, rayInfo};

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
                updateObjects(uiParams.rmlPath.string(), rObjects);
                for (auto& rObj : rObjects) {
                    rObj.updateTexture(canonicalizeRepositoryPath("Intern/rayx-ui/res/textures/white.png"), *m_DescriptorPool);
                }
                createRayCache(uiParams.rmlPath.string(), rayCache, uiParams.rayInfo);
                uiParams.pathChanged = false;
                camController.lookAtPoint(rObjects[0].getTranslationVecor());
                uiParams.rayInfo.raysChanged = true;
            }

            if (uiParams.rayInfo.raysChanged) {
                if (uiParams.rayInfo.cacheChanged) {
                    createRayCache(uiParams.rmlPath.string(), rayCache, uiParams.rayInfo);
                    uiParams.rayInfo.cacheChanged = false;
                }
                if (uiParams.rayInfo.displayRays) {
                    updateRays(uiParams.rmlPath.string(), rayCache, rayObj, rays, uiParams.rayInfo);
                } else {
                    rayObj.reset();
                }
                uiParams.rayInfo.raysChanged = false;
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

void Application::updateObjects(const std::string& path, std::vector<RenderObject>& rObjects) {
    RAYX::Beamline beamline = RAYX::importBeamline(path);
    // TODO(Jannis): Hacky fix for now; should be some form of synchronization
    vkDeviceWaitIdle(m_Device.device());
    // Triangulate the render data and update the scene
    rObjects = RenderObject::buildRObjectsFromElements(m_Device, beamline.m_OpticalElements);
}

void Application::createRayCache(const std::string& path, BundleHistory& rayCache, UIRayInfo& rayInfo) {
#ifndef NO_H5
    std::string rayFilePath = path.substr(0, path.size() - 4) + ".h5";
    RAYX::BundleHistory bundleHist = raysFromH5(rayFilePath, FULL_FORMAT);
#else
    std::string rayFilePath = path.substr(0, path.size() - 4) + ".csv";
    RAYX::BundleHistory bundleHist = loadCSV(rayFilePath);
#endif
    rayInfo.maxAmountOfRays = (int)bundleHist.size();
    if (rayInfo.renderAllRays) {
        rayCache = bundleHist;
        return;
    }
    const size_t m = getMaxEvents(bundleHist);

    std::vector<size_t> indices(bundleHist.size());
    std::iota(indices.begin(), indices.end(), 0);  // Filling indices with 0, 1, 2, ..., n-1

    // Randomly shuffling the indices
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::shuffle(indices.begin(), indices.end(), engine);

    std::unordered_set<size_t> selectedIndices;

    // Selecting rays for each event index
    for (size_t eventIdx = 0; eventIdx < m; ++eventIdx) {
        size_t count = 0;
        for (size_t rayIdx : indices) {
            if (count >= MAX_RAYS) break;
            if (bundleHist[rayIdx].size() > eventIdx) {
                selectedIndices.insert(rayIdx);
                count++;
            }
        }
    }

    rayCache.clear();
    // Now selectedIndices contains unique indices of rays
    // Creating rayCache object from selected indices
    for (size_t idx : selectedIndices) {
        rayCache.push_back(bundleHist[idx]);
    }

    rayInfo.maxAmountOfRays = (int)rayCache.size();
}

void Application::updateRays(const std::string& path, BundleHistory& rayCache, std::optional<RenderObject>& rayObj, std::vector<Line>& rays,
                             UIRayInfo& rayInfo) {
    RAYX::Beamline beamline = RAYX::importBeamline(path);
    if (!rayInfo.renderAllRays) {
        rays = getRays(rayCache, beamline.m_OpticalElements, kMeansFilter, rayInfo.amountOfRays);
    } else {
        rays = getRays(rayCache, beamline.m_OpticalElements, noFilter, rayInfo.maxAmountOfRays);
    }
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
