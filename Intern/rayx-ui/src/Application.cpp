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
    m_TexturePool = nullptr;

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
    GridRenderSystem gridRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());

    auto currentTime = std::chrono::high_resolution_clock::now();
    std::vector<glm::dvec3> rSourcePositions;
    std::vector<RAYX::OpticalElement> elements;
    std::vector<std::shared_ptr<RAYX::LightSource>> sources;

    std::future<void> beamlineFuture;
    std::future<void> raysFuture;
    std::future<void> buildRayCacheFuture;
    std::future<void> simulationFuture;
    std::future<std::vector<Scene::RenderObjectInput>> getRObjInputsFuture;

    // Main loop
    m_State = State::RunningWithoutScene;
    while (!m_Window.shouldClose()) {
        // Skip rendering when minimized
        if (m_Window.isMinimized()) {
            continue;
        }
        glfwPollEvents();

        if (auto commandBuffer = m_Renderer.beginFrame()) {
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
                        RAYX_VERB << "Loaded H5 file: " << m_RMLPath.string().substr(0, m_RMLPath.string().size() - 4) + ".h5";

                        elements = m_Beamline->m_OpticalElements;
                        sources = m_Beamline->m_LightSources;
                        rSourcePositions.clear();
                        for (auto& source : sources) {
                            rSourcePositions.push_back(source->getPosition());
                        }

                        m_TexturePool = DescriptorPool::Builder(m_Device)
                                            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(elements.size()) + 1)
                                            .setMaxSets(static_cast<uint32_t>(elements.size()) + 1)
                                            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
                                            .build();
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
                        m_Scene->buildRaysRObject(*m_Beamline, m_UIParams.rayInfo, texSetLayout, m_TexturePool);
                        if (m_Scene->getState() == Scene::State::Complete) {
                            m_State = State::Running;
                        } else {
                            m_State = State::BuildingElements;
                        }
                    }
                    break;
                case State::BuildingElements:
                    if (getRObjInputsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                        m_Scene->buildRObjectsFromInput(getRObjInputsFuture.get(), texSetLayout, m_TexturePool);
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
            m_Renderer.beginSwapChainRenderPass(commandBuffer, m_UIHandler.getClearValue());

            FrameInfo frameInfo{m_Camera, frameIndex, commandBuffer, descriptorSets[frameIndex]};

            // Scene
            // only LoadingRays check really necessary but this is for better user experience
            if (m_Scene && State::LoadingRays != m_State && m_State != State::InitializeSimulation && m_State != State::Simulating) {
                objectRenderSystem.render(frameInfo, m_Scene->getRObjects());
                if (m_UIParams.rayInfo.displayRays) rayRenderSystem.render(frameInfo, m_Scene->getRaysRObject());
            }

            // Grid
            gridRenderSystem.render(frameInfo);

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
    m_rays = raysFromH5(rayFilePath, FULL_FORMAT, m_UIParams.rayInfo.startEventID);
#else
    std::string rayFilePath = rmlPath.string().substr(0, rmlPath.string().size() - 4) + ".csv";
    m_rays = loadCSV(rayFilePath);
#endif
}

void Application::loadBeamline(const std::filesystem::path& rmlPath) {
    m_Beamline = std::make_unique<RAYX::Beamline>(RAYX::importBeamline(rmlPath));
    m_UIParams.simulationInfo.maxEvents = static_cast<int>(m_Beamline->m_OpticalElements.size()) + 2;
}