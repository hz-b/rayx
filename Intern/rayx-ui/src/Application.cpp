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
#include "Plotting.h"
#include "RayProcessing.h"
#include "RenderObject.h"
#include "RenderSystem/GridRenderSystem.h"
#include "RenderSystem/ObjectRenderSystem.h"
#include "RenderSystem/RayRenderSystem.h"
#include "Triangulation/GeometryUtils.h"
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
    m_GlobalDescriptorPool = DescriptorPool::Builder(m_Device)
                                 .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .build();
    m_TexturePool = nullptr;
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

    // Descriptor set layouts
    auto globalSetLayout = DescriptorSetLayout::Builder(m_Device)
                               .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  //
                               .build();                                                                      //
    std::vector<VkDescriptorSet> descriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (unsigned long i = 0; i < descriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *m_GlobalDescriptorPool).writeBuffer(0, &bufferInfo).build(descriptorSets[i]);
    }
    std::shared_ptr<DescriptorSetLayout> texSetLayout =
        DescriptorSetLayout::Builder(m_Device)                                                       //
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)  //
            .build();
    std::vector<VkDescriptorSetLayout> setLayouts{globalSetLayout->getDescriptorSetLayout(), texSetLayout->getDescriptorSetLayout()};

    // Render systems
    ObjectRenderSystem objectRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass(), setLayouts);
    RayRenderSystem rayRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
    UIRenderSystem uiRenderSystem(m_Window, m_Device, m_Renderer.getSwapChainImageFormat(), m_Renderer.getSwapChainDepthFormat(),
                                  m_Renderer.getSwapChainImageCount());
    GridRenderSystem gridRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());

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
    std::vector<glm::dvec3> rSourcePositions;
    std::vector<Line> rays;
    BundleHistory rayCache;
    std::optional<RenderObject> rayObj;

    // CLI Input
    std::string rmlPathCli = m_CommandParser.m_args.m_providedFile;
    UIRayInfo rayInfo{
        .displayRays = true,     //
        .raysChanged = false,    //
        .cacheChanged = false,   //
        .renderAllRays = false,  //
        .amountOfRays = 50,      //
        .maxAmountOfRays = 100   //
    };
    UIParameters uiParams{
        .camController = camController,      //
        .rmlPath = rmlPathCli,               //
        .pathChanged = !rmlPathCli.empty(),  //
        .frameTime = 0.0,                    //
        .rayInfo = rayInfo,
        .showRMLNotExistPopup = false,
        .showH5NotExistPopup = false,
        .pathValidState = false  //
    };

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
            uiRenderSystem.setupUI(uiParams, rObjects, rSourcePositions);
            // camController.update(cam, m_Renderer.getAspectRatio());

            if (uiParams.pathChanged) {
                std::string rmlPath = uiParams.rmlPath.string();

                if (!uiParams.showH5NotExistPopup && !uiParams.showRMLNotExistPopup) {
                    vkDeviceWaitIdle(m_Device.device());
                    m_Beamline = RAYX::importBeamline(rmlPath);
                    std::vector<RAYX::OpticalElement> elements = m_Beamline.m_OpticalElements;
                    rSourcePositions.clear();
                    for (auto& source : m_Beamline.m_LightSources) {
                        rSourcePositions.push_back(glm::dvec3((*source).getPosition()));
                    }

                    m_TexturePool = DescriptorPool::Builder(m_Device)
                                        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, (uint32_t)elements.size())
                                        .setMaxSets((uint32_t)elements.size())
                                        .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
                                        .build();
                    rObjects = RenderObject::buildRObjectsFromElements(m_Device, elements, texSetLayout, m_TexturePool);

                    setLayouts = {globalSetLayout->getDescriptorSetLayout(), texSetLayout->getDescriptorSetLayout()};
                    objectRenderSystem.rebuild(m_Renderer.getSwapChainRenderPass(), setLayouts);

                    createRayCache(uiParams.rmlPath.string(), rayCache, uiParams.rayInfo);
                    uiParams.pathChanged = false;
                    camController.lookAtPoint(rObjects[0].getTranslationVecor());
                    uiParams.rayInfo.raysChanged = true;

                    for (uint32_t i = 0; i < elements.size(); i++) {
                        if (rObjects[i].getVertexCount() == 4) {
                            auto [width, height] = getRectangularDimensions(elements[i].m_element.m_cutout);

                            auto raysOfElement = [&](uint32_t elementIdx) {
                                std::vector<RAYX::Ray> rays;
                                for (auto& rayHist : m_rays) {
                                    for (auto& ray : rayHist) {
                                        if (ray.m_lastElement == elementIdx) {
                                            rays.push_back(ray);
                                        }
                                    }
                                }
                                return rays;
                            };
                            std::vector<std::vector<uint32_t>> footprint = makeFootprint(raysOfElement(i), -width / 2, width / 2, -height / 2,
                                                                                         height / 2, (uint32_t)(width * 10), (uint32_t)(height * 10));
                            uint32_t footprintWidth, footprintHeight;
                            unsigned char* data = footprintAsImage(footprint, footprintWidth, footprintHeight);
                            rObjects[i].updateTexture(data, footprintWidth, footprintHeight);
                        }
                    }
                }
            }
            uiParams.pathChanged = false;

            if (uiParams.rayInfo.raysChanged) {
                vkDeviceWaitIdle(m_Device.device());
                if (uiParams.rayInfo.cacheChanged) {
                    createRayCache(uiParams.rmlPath.string(), rayCache, uiParams.rayInfo);
                    uiParams.rayInfo.cacheChanged = false;
                }
                if (uiParams.rayInfo.displayRays) {
                    updateRays(rayCache, rayObj, rays, uiParams.rayInfo);
                } else {
                    rayObj.reset();
                }
                uiParams.rayInfo.raysChanged = false;
            }

            camController.update(cam, m_Renderer.getAspectRatio());

            // Update UBO
            uint32_t frameIndex = m_Renderer.getFrameIndex();
            uboBuffers[frameIndex]->writeToBuffer(&cam);

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

void Application::createRayCache(const std::string& path, BundleHistory& rayCache, UIRayInfo& rayInfo) {
    loadRays(path);
    rayInfo.maxAmountOfRays = (int)m_rays.size();
    if (rayInfo.renderAllRays) {
        rayCache = m_rays;
        return;
    }
    const size_t m = getMaxEvents(m_rays);

    std::vector<size_t> indices(m_rays.size());
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
            if (m_rays[rayIdx].size() > eventIdx) {
                selectedIndices.insert(rayIdx);
                count++;
            }
        }
    }

    rayCache.clear();
    // Now selectedIndices contains unique indices of rays
    // Creating rayCache object from selected indices
    for (size_t idx : selectedIndices) {
        rayCache.push_back(m_rays[idx]);
    }

    rayInfo.maxAmountOfRays = (int)rayCache.size();
}

void Application::loadRays(const std::string& rmlPath) {
    vkDeviceWaitIdle(m_Device.device());
#ifndef NO_H5
    std::string rayFilePath = rmlPath.substr(0, rmlPath.size() - 4) + ".h5";
    m_rays = raysFromH5(rayFilePath, FULL_FORMAT);
#else
    std::string rayFilePath = rmlPath.substr(0, rmlPath.size() - 4) + ".csv";
    m_rays = loadCSV(rayFilePath);
#endif
}

void Application::updateRays(BundleHistory& rayCache, std::optional<RenderObject>& rayObj, std::vector<Line>& rays, UIRayInfo& rayInfo) {
    if (!rayInfo.renderAllRays) {
        rays = getRays(rayCache, m_Beamline.m_OpticalElements, kMeansFilter, (uint32_t)rayInfo.amountOfRays);
    } else {
        rays = getRays(rayCache, m_Beamline.m_OpticalElements, noFilter, (uint32_t)rayInfo.maxAmountOfRays);
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

        std::shared_ptr<DescriptorPool> rayDescrPool =
            DescriptorPool::Builder(m_Device).addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1).setMaxSets(1).build();
        std::shared_ptr<DescriptorSetLayout> raySetLayout =
            DescriptorSetLayout::Builder(m_Device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
        rayObj.emplace("Rays", m_Device, glm::mat4(1.0f), rayVertices, rayIndices, raySetLayout, rayDescrPool);
    }
}
