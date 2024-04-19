#include "Application.h"

#include <chrono>
#include <future>

#include "CanonicalizePath.h"
#include "Colors.h"
#include "Data/Importer.h"
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
#include "Triangulation/GeometryUtils.h"
#include "UserInput.h"
#include "Writer/CSVWriter.h"
#include "Writer/H5Writer.h"

bool isSceneWindowHovered = false;  // TODO: remove this global variable (doing this will require a refactor of the way we use glfw)

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
    m_GlobalDescriptorPool = DescriptorPool::Builder(m_Device)
                                 .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .build();
    m_TexturePool = DescriptorPool::Builder(m_Device)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
                        .setMaxSets(1000)
                        .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
                        .build();

    init();
}

Application::~Application() = default;

void Application::init() {
    RAYX_PROFILE_FUNCTION_STDOUT();

    m_CommandParser.analyzeCommands();
    if (m_CommandParser.m_args.m_benchmark) {
        RAYX_VERB << "Starting in Benchmark Mode.\n";
        RAYX::BENCH_FLAG = true;
    }

    std::string rmlPathCli = m_CommandParser.m_args.m_providedFile;
    UIRayInfo rayInfo{
        .displayRays = true,     //
        .raysChanged = false,    //
        .cacheChanged = false,   //
        .renderAllRays = false,  //
        .amountOfRays = 50,      //
        .maxAmountOfRays = 100   //
    };

    m_UIParams.updatePath(rmlPathCli);
    m_UIParams.rayInfo = rayInfo;

    glfwSetKeyCallback(m_Window.window(), keyCallback);
    glfwSetMouseButtonCallback(m_Window.window(), mouseButtonCallback);
    glfwSetCursorPosCallback(m_Window.window(), cursorPosCallback);
    glfwSetWindowUserPointer(m_Window.window(), &m_CamController);
}

void Application::run() {
    // Create UBOs (Uniform Buffer Object)
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (auto& uboBuffer : uboBuffers) {
        uboBuffer = std::make_unique<Buffer>(m_Device, "uboBuffer", sizeof(Camera), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffer->map();
    }

    auto globalSetLayout = DescriptorSetLayout::Builder(m_Device)
                               .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  //
                               .build();
    std::vector<VkDescriptorSet> descriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (unsigned long i = 0; i < descriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *m_GlobalDescriptorPool).writeBuffer(0, &bufferInfo).build(descriptorSets[i]);
    }
    auto textureSetLayout =
        DescriptorSetLayout::Builder(m_Device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
    // Init render systems
    std::vector<VkDescriptorSetLayout> layouts = {globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout()};
    GridRenderSystem gridRenderSystem(m_Device, m_Renderer.getOffscreenRenderPass(), globalSetLayout->getDescriptorSetLayout());
    ObjectRenderSystem objectRenderSystem(m_Device, m_Renderer.getOffscreenRenderPass(), layouts);
    RayRenderSystem rayRenderSystem(m_Device, m_Renderer.getOffscreenRenderPass(), globalSetLayout->getDescriptorSetLayout());

    auto currentTime = std::chrono::high_resolution_clock::now();
    std::vector<glm::dvec3> rSourcePositions;
    std::vector<RAYX::DesignElement> elements;
    std::vector<RAYX::DesignSource> sources;

    std::future<void> beamlineFuture;
    std::future<void> raysFuture;
    std::future<void> buildRayCacheFuture;
    std::future<void> simulationFuture;
    std::future<std::vector<Scene::RenderObjectInput>> getRObjInputsFuture;
    // Main loop
    VkExtent2D sceneExtent = {1920, 1080};
    m_State = State::RunningWithoutScene;
    while (!m_Window.shouldClose()) {
        // Skip rendering when minimized
        if (m_Window.isMinimized()) {
            continue;
        }
        glfwPollEvents();

        if (VkCommandBuffer commandBuffer = m_Renderer.beginFrame()) {
            // Params to pass to UI
            auto newTime = std::chrono::high_resolution_clock::now();
            m_UIParams.frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            if (m_UIParams.rmlReady) {
                m_RMLPath = m_UIParams.rmlPath.string();
                beamlineFuture = std::async(std::launch::async, &Application::loadBeamline, this, m_RMLPath);
                m_State = State::LoadingBeamline;
                m_UIParams.rmlReady = false;
            }

            switch (m_State) {
                case State::LoadingBeamline:
                    if (beamlineFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                        if (m_UIParams.runSimulation) {
                            if (m_UIParams.simulationSettingsReady) {
                                // open simulation dialog
                                m_Simulator.setSimulationParameters(m_RMLPath, *m_Beamline, m_UIParams.simulationInfo);
                                m_UIParams.simulationSettingsReady = false;
                                m_State = State::InitializeSimulation;
                            }
                        } else {
                            raysFuture = std::async(std::launch::async, &Application::loadRays, this, m_RMLPath);
                            m_State = State::LoadingRays;
                        }
                    }
                    break;

                case State::InitializeSimulation:
                    simulationFuture = std::async(std::launch::async, std::bind(&Simulator::runSimulation, &m_Simulator));
                    m_State = State::Simulating;
                    m_UIParams.runSimulation = false;
                    break;

                case State::Simulating:
                    if (simulationFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                        raysFuture = std::async(std::launch::async, &Application::loadRays, this, m_RMLPath);
                        m_State = State::LoadingRays;
                    }
                    break;
                case State::LoadingRays:
                    if (beamlineFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready &&
                        raysFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                        // Wait for loadBeamline and loadRays to finish
                        RAYX_VERB << "Loaded RML file: " << m_RMLPath;

                        for (auto ray : m_rays) {
                            // get last elemtent of vector
                            size_t id = static_cast<size_t>(ray.back().m_lastElement);
                            if (id > m_Beamline->m_DesignElements.size()) {
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

                        elements = m_Beamline->m_DesignElements;
                        sources = m_Beamline->m_DesignSources;
                        rSourcePositions.clear();
                        for (auto& source : sources) {
                            rSourcePositions.push_back(source.getWorldPosition());
                        }

                        m_Scene = std::make_unique<Scene>(m_Device);

                        buildRayCacheFuture =
                            std::async(std::launch::async, &Scene::buildRayCache, m_Scene.get(), std::ref(m_UIParams.rayInfo), std::ref(m_rays));
                        getRObjInputsFuture =
                            std::async(std::launch::async, &Scene::getRObjectInputs, m_Scene.get(), std::ref(elements), std::ref(m_rays));

                        m_State = State::BuildingRays;
                    }
                    break;
                case State::BuildingRays:
                    if (buildRayCacheFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                        m_Scene->buildRaysRObject(*m_Beamline, m_UIParams.rayInfo, textureSetLayout, m_TexturePool);
                        if (m_Scene->getState() == Scene::State::Complete) {
                            m_State = State::Running;
                        } else {
                            m_State = State::BuildingElements;
                        }
                    }
                    break;
                case State::BuildingElements:
                    if (getRObjInputsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                        m_Scene->buildRObjectsFromInput(getRObjInputsFuture.get(), textureSetLayout, m_TexturePool);
                        m_State = State::Running;
                    }
                    break;
                case State::Running:
                    break;
                default:
                    break;
            }

            if (m_UIParams.rayInfo.cacheChanged) {
                buildRayCacheFuture =
                    std::async(std::launch::async, &Scene::buildRayCache, m_Scene.get(), std::ref(m_UIParams.rayInfo), std::ref(m_rays));
                m_State = State::BuildingRays;
                m_UIParams.rayInfo.cacheChanged = false;
            }

            if (m_UIParams.rayInfo.raysChanged) {
                m_State = State::BuildingRays;
                m_UIParams.rayInfo.raysChanged = false;
            }

            // Update UBO
            uint32_t frameIndex = m_Renderer.getFrameIndex();
            m_CamController.update(m_Camera, m_Renderer.getAspectRatio());
            uboBuffers[frameIndex]->writeToBuffer(&m_Camera);

            // Render
            FrameInfo frameInfo = {
                .camera = m_Camera,                          //
                .frameIndex = frameIndex,                    //
                .sceneExtent = m_UIParams.sceneExtent,       //
                .commandBuffer = VK_NULL_HANDLE,             //
                .descriptorSet = descriptorSets[frameIndex]  //
            };
            if (m_UIParams.sceneExtent.height != 0 && m_UIParams.sceneExtent.width != 0) {
                if (m_UIParams.sceneExtent.height != sceneExtent.height || m_UIParams.sceneExtent.width != sceneExtent.width) {
                    m_Renderer.resizeOffscreenResources(m_UIParams.sceneExtent);
                    m_Renderer.offscreenDescriptorSetUpdate(*textureSetLayout, *m_TexturePool, m_UIParams.sceneDescriptorSet);
                }
            }
            m_Renderer.beginOffscreenRenderPass(frameInfo);
            if (m_Scene && State::LoadingRays != m_State && m_State != State::InitializeSimulation && m_State != State::Simulating) {
                objectRenderSystem.render(frameInfo, m_Scene->getRObjects());
                if (m_UIParams.rayInfo.displayRays) rayRenderSystem.render(frameInfo, m_Scene->getRaysRObject());
            }
            gridRenderSystem.render(frameInfo);
            m_Renderer.endOffscreenRenderPass(frameInfo);

            m_Renderer.beginSwapChainRenderPass(commandBuffer, m_UIHandler.getClearValue());
            // UI
            m_UIHandler.beginUIRender();
            m_UIHandler.setupUI(m_UIParams, elements, rSourcePositions);
            m_UIHandler.endUIRender(commandBuffer);

            m_Renderer.endSwapChainRenderPass(commandBuffer);
            m_Renderer.endFrame();
        } else {
            RAYX_LOG << "Failed to acquire swap chain image";
            break;
        }
    }
    vkDeviceWaitIdle(m_Device.device());
}

void Application::loadRays(const std::filesystem::path& rmlPath) {
    RAYX_PROFILE_FUNCTION_STDOUT();
#ifndef NO_H5
    std::string rayFilePath = rmlPath.string().substr(0, rmlPath.string().size() - 4) + ".h5";
    m_rays = raysFromH5(rayFilePath, FULL_FORMAT, &m_UIParams.rayInfo.startEventID);

#else
    std::string rayFilePath = rmlPath.string().substr(0, rmlPath.string().size() - 4) + ".csv";
    m_rays = loadCSV(rayFilePath);
#endif
}

void Application::loadBeamline(const std::filesystem::path& rmlPath) {
    m_Beamline = std::make_unique<RAYX::Beamline>(RAYX::importBeamline(rmlPath));
    m_UIParams.simulationInfo.maxEvents = static_cast<unsigned int>(m_Beamline->m_DesignElements.size()) + 2;
}