#include "Application.h"

#include <SDL3/SDL.h>

#include <chrono>
#include <future>

#include "CanonicalizePath.h"
#include "Colors.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "FrameInfo.h"
#include "GraphicsCore/Renderer.h"
#include "GraphicsCore/Window.h"
#include "RayProcessing.h"
#include "RenderObject.h"
#include "RenderSystem/GridRenderSystem.h"
#include "RenderSystem/ObjectRenderSystem.h"
#include "RenderSystem/RayRenderSystem.h"
#include "Rml/Importer.h"
#include "Triangulation/GeometryUtils.h"
#include "UserInput.h"
#include "Writer/CsvWriter.h"
#include "Writer/H5Writer.h"

// --------- Start of Application code --------- //
Application::Application(uint32_t width, uint32_t height, const char* name, int argc, char** argv)
    : m_Window(width, height, name),                                   //
      m_CommandParser(argc, argv),                                     //
      m_Device(m_Window, m_CommandParser.m_args.m_deviceID),           //
      m_Renderer(m_Window, m_Device),                                  //
      m_Simulator(),                                                   //
      m_Camera(),                                                      //
      m_CamController(),                                               //
      m_UIParams(m_CamController, m_Simulator.getAvailableDevices()),  //
      m_UIHandler(m_Window, m_Device, m_Renderer.getSwapChainImageFormat(), m_Renderer.getSwapChainDepthFormat(),
                  m_Renderer.getSwapChainImageCount()) {
    init();
}

Application::~Application() = default;

void Application::init() {
    RAYX_PROFILE_FUNCTION_STDOUT();

    m_GlobalDescriptorPool = DescriptorPool::Builder(m_Device)
                                 .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .build();
    m_TexturePool = DescriptorPool::Builder(m_Device)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
                        .setMaxSets(1000)
                        .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
                        .build();

    std::string rmlPathCli = m_CommandParser.m_args.m_providedFile;
    UIRayInfo rayInfo{
        .raysLoaded      = false,  //
        .displayRays     = true,   //
        .raysChanged     = false,  //
        .cacheChanged    = false,  //
        .renderAllRays   = false,  //
        .amountOfRays    = 50,     //
        .maxAmountOfRays = 100     //
    };

    m_UIParams.updatePath(rmlPathCli);
    m_UIParams.rayInfo = rayInfo;

    // glfwSetKeyCallback(m_Window.window(), keyCallback);
    // glfwSetMouseButtonCallback(m_Window.window(), mouseButtonCallback);
    // glfwSetCursorPosCallback(m_Window.window(), cursorPosCallback);
    // Create UBOs (Uniform Buffer Object)
    m_uboBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (auto& uboBuffer : m_uboBuffers) {
        uboBuffer = std::make_unique<Buffer>(m_Device, "uboBuffer", sizeof(Camera), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffer->map();
    }

    // ? Should render systems handle their own descriptors?  Pool, set layout and sets are not really used outside
    // apart from registering textures
    m_globalSetLayout = DescriptorSetLayout::Builder(m_Device)
                            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  //
                            .build();
    for (auto i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        auto bufferInfo = m_uboBuffers[i]->descriptorInfo();
        VkDescriptorSet set;
        DescriptorWriter(*m_globalSetLayout, *m_GlobalDescriptorPool).writeBuffer(0, &bufferInfo).build(set);
        m_descriptorSets.push_back(std::move(set));
    }
    m_textureSetLayout =
        DescriptorSetLayout::Builder(m_Device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
    // Init render systems
    m_gridRenderSystem   = std::make_unique<GridRenderSystem>(m_Device,                             //
                                                              m_Renderer.getOffscreenRenderPass(),  //
                                                              std::vector{m_globalSetLayout->getDescriptorSetLayout()});
    m_objectRenderSystem = std::make_unique<ObjectRenderSystem>(m_Device,                                                 //
                                                                m_Renderer.getOffscreenRenderPass(),                      //
                                                                std::vector{m_globalSetLayout->getDescriptorSetLayout(),  //
                                                                            m_textureSetLayout->getDescriptorSetLayout()});
    m_rayRenderSystem    = std::make_unique<RayRenderSystem>(m_Device,                             //
                                                             m_Renderer.getOffscreenRenderPass(),  //
                                                             std::vector{m_globalSetLayout->getDescriptorSetLayout()});

    m_State = State::RunningWithoutScene;  // glfwSetWindowUserPointer(m_Window.window(), &m_CamController);
}

void Application::run() {
    if (VkCommandBuffer commandBuffer = m_Renderer.beginFrame()) {
        if (m_UIParams.rmlReady) {
            m_RMLPath               = m_UIParams.rmlPath.string();
            m_UIParams.beamlineInfo = {};
            m_beamlineFuture        = std::async(std::launch::async, &Application::loadBeamline, this, m_RMLPath);
            m_State                 = State::LoadingBeamline;
            m_UIParams.rmlReady     = false;
        }
        if (m_UIParams.runSimulation) {
            if (m_UIParams.simulationSettingsReady) {
                // open simulation dialog
                m_Simulator.setSimulationParameters(m_RMLPath, *m_Beamline, m_UIParams.simulationInfo);
                m_UIParams.simulationSettingsReady = false;
                m_State                            = State::InitializeSimulation;
            }
        }

        switch (m_State) {
            case State::LoadingBeamline:
                if (m_beamlineFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    // Clear existing rays and reset scene
                    m_rays.clear();
                    m_UIParams.rayInfo.raysLoaded = false;
                    m_Scene                       = std::make_unique<Scene>(m_Device);

                    // Update elements and sources for UI
                    m_UIParams.beamlineInfo.beamline = m_Beamline.get();

                    // Prepare for ray loading or element preparation
                    size_t numSources  = m_Beamline->numSources();
                    size_t numElements = m_Beamline->numElements();
                    m_sortedRays.resize(numElements);
                    if (m_UIParams.h5Ready) {
                        m_raysFuture = std::async(std::launch::async, &Application::loadRays, this, m_RMLPath, numSources, numElements);
                        m_State      = State::LoadingRays;
                    } else {
                        m_State = State::PrepareElements;
                    }
                }
                break;

            case State::InitializeSimulation:
                m_simulationFuture       = std::async(std::launch::async, std::bind(&Simulator::runSimulation, &m_Simulator));
                m_State                  = State::Simulating;
                m_UIParams.runSimulation = false;
                break;

            case State::Simulating:
                if (m_simulationFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    m_raysFuture =
                        std::async(std::launch::async, &Application::loadRays, this, m_RMLPath, m_Beamline->numSources(), m_Beamline->numElements());
                    m_State = State::LoadingRays;
                }
                break;
            case State::LoadingRays:
                if (m_beamlineFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready &&
                    m_raysFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    // Wait for loadBeamline and loadRays to finish
                    RAYX_VERB << "Loaded RML file: " << m_RMLPath;
                    if (m_rays.size() == 0) {
                        if (m_buildElementsNeeded) {
                            m_buildTextureNeeded = true;
                            m_State              = State::PrepareElements;
                        } else {
                            m_State               = State::Running;
                            m_buildElementsNeeded = true;
                        }
                    } else {
                        for (auto ray : m_rays) {
                            size_t id = static_cast<size_t>(ray.back().m_lastElement);
                            if (id > m_Beamline->numElements()) {
                                m_UIParams.showH5NotExistPopup = true;
                                break;
                            }
                        }
                        if (m_UIParams.showH5NotExistPopup) {
                            RAYX_VERB << "H5 file not compatible with RML file";
                            m_State = State::RunningWithoutScene;
                            break;
                        }

                        RAYX_VERB << "Loaded H5 file: " << m_RMLPath.string().substr(0, m_RMLPath.string().size() - 4) + ".h5";
                        // We do not need to check the future state here as the loop will not come back here until user
                        // interacts with UI again
                        m_buildRayCacheFuture =
                            std::async(std::launch::async, &Scene::buildRayCache, m_Scene.get(), std::ref(m_UIParams.rayInfo), std::ref(m_rays));
                        m_State = State::BuildingRays;
                    }
                }
                break;
            case State::BuildingRays:
                if (m_buildRayCacheFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    m_Scene->buildRaysRObject(*m_Beamline, m_UIParams.rayInfo, m_textureSetLayout, m_TexturePool);
                    m_UIParams.rayInfo.raysLoaded = true;
                    if (m_buildElementsNeeded) {
                        m_buildTextureNeeded = true;
                        m_State              = State::PrepareElements;
                    } else {
                        m_State               = State::Running;
                        m_buildElementsNeeded = true;
                    }
                }
                break;
            case State::PrepareElements:
                // only start async task if one is not already running
                if (!m_getRObjInputsFuture.valid() ||
                    (m_getRObjInputsFuture.valid() && m_getRObjInputsFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)) {
                    m_getRObjInputsFuture = std::async(std::launch::async, &Scene::getRObjectInputs, m_Scene.get(), std::ref(*m_Beamline),
                                                       m_sortedRays, m_buildTextureNeeded);
                } else {
                    RAYX_VERB << "Skipping PrepareElements, async task already running.";
                }

                m_State = State::BuildingElements;
                break;
            case State::BuildingElements:
                if (m_getRObjInputsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    m_Scene->buildRObjectsFromInput(m_getRObjInputsFuture.get(), m_textureSetLayout, m_TexturePool, m_buildTextureNeeded);
                    m_buildTextureNeeded = false;
                    m_State              = State::Running;
                }
                break;
            case State::Running:
                break;
            default:
                break;
        }

        if (m_UIParams.rayInfo.cacheChanged) {
            // only start async task if one is not already running
            if (m_buildRayCacheFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                m_buildRayCacheFuture =
                    std::async(std::launch::async, &Scene::buildRayCache, m_Scene.get(), std::ref(m_UIParams.rayInfo), std::ref(m_rays));
                m_UIParams.rayInfo.cacheChanged = false;
                m_State                         = State::BuildingRays;
            }
        }

        if (m_UIParams.rayInfo.raysChanged) {
            m_State                        = State::BuildingRays;
            m_UIParams.rayInfo.raysChanged = false;
            m_buildElementsNeeded          = false;
            m_buildTextureNeeded           = true;
        }
        if (m_UIParams.beamlineInfo.elementsChanged) {
            m_State                                 = State::PrepareElements;
            m_UIParams.beamlineInfo.elementsChanged = false;
        }

        // Update UBO
        uint32_t frameIndex = m_Renderer.getFrameIndex();
        m_CamController.update(m_Camera, m_Renderer.getAspectRatio());
        m_uboBuffers[frameIndex]->writeToBuffer(&m_Camera);

        // Render
        FrameInfo frameInfo = {
            .camera        = m_Camera,                     //
            .frameIndex    = frameIndex,                   //
            .sceneExtent   = m_UIParams.sceneExtent,       //
            .commandBuffer = VK_NULL_HANDLE,               //
            .descriptorSet = m_descriptorSets[frameIndex]  //
        };
        if (m_UIParams.sceneExtent.height != sceneExtent.height || m_UIParams.sceneExtent.width != sceneExtent.width) {
            m_Renderer.resizeOffscreenResources(m_UIParams.sceneExtent);
            m_Renderer.offscreenDescriptorSetUpdate(*m_textureSetLayout, *m_TexturePool, m_UIParams.sceneDescriptorSet);
            sceneExtent = m_UIParams.sceneExtent;
        }
        m_Renderer.beginOffscreenRenderPass(frameInfo, m_UIHandler.getClearValue());
        if (m_Scene && State::LoadingRays != m_State && m_State != State::InitializeSimulation && m_State != State::Simulating) {
            m_objectRenderSystem->render(frameInfo, m_Scene->getRObjects());
            if (m_UIParams.rayInfo.displayRays) m_rayRenderSystem->render(frameInfo, m_Scene->getRaysRObject());
        }
        m_gridRenderSystem->render(frameInfo, {});
        m_Renderer.endOffscreenRenderPass(frameInfo);

        m_Renderer.beginSwapChainRenderPass(commandBuffer);
        // UI
        m_UIHandler.beginUIRender();
        m_UIHandler.setupUI(m_UIParams);
        m_UIHandler.endUIRender(commandBuffer);

        m_Renderer.endSwapChainRenderPass(commandBuffer);
        m_Renderer.endFrame();
    } else {
        RAYX_LOG << "Failed to acquire swap chain image";
        return;
    }
    vkDeviceWaitIdle(m_Device.device());  // TODO: check if necessary and if yes remove and find better solution
}

void Application::handleEvent(const SDL_Event* event) {
    UserInputContext context = {
        .camController        = &m_CamController,
        .isSceneWindowHovered = m_UIParams.isSceneWindowHovered,
    };
    switch (event->type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            keyEventHandler(event, context);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouseButtonEventHandler(event, context);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            mouseMotionEventHandler(event, context);
            break;
        default:
            return;
    }
}

void Application::loadRays(const std::filesystem::path& rmlPath, const size_t numSources, const size_t numElements) {
    RAYX_PROFILE_FUNCTION_STDOUT();
#ifndef NO_H5
    std::string rayFilePath = rmlPath.string().substr(0, rmlPath.string().size() - 4) + ".h5";
    m_rays                  = convertRaysToBundleHistory(rayx::readH5Rays(rayFilePath), static_cast<int>(numSources));

#else
    std::string rayFilePath = rmlPath.string().substr(0, rmlPath.string().size() - 4) + ".csv";
    m_rays                  = convertRaysToBundleHistory(rayx::readCsv(rayFilePath), static_cast<int>(numSources));
#endif
    sortRaysByElement(m_rays, m_sortedRays, numElements);
}

void Application::loadBeamline(const std::filesystem::path& rmlPath) {
    m_Beamline                          = std::make_unique<rayx::Beamline>(rayx::importBeamline(rmlPath));
    m_UIParams.simulationInfo.maxEvents = rayx::defaultMaxEvents(m_Beamline->numObjects());
}
