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
}

void Application::run() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::vector<glm::dvec3> rSourcePositions;  // TODO: how to handle these two?
    std::vector<RAYX::OpticalElement> elements;

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

            // Render
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
    m_rays = raysFromH5(rayFilePath, FULL_FORMAT);
#else
    std::string rayFilePath = rmlPath.string().substr(0, rmlPath.string().size() - 4) + ".csv";
    m_rays = loadCSV(rayFilePath);
#endif
}

void Application::loadBeamline(const std::filesystem::path& rmlPath) { m_Beamline = std::make_unique<RAYX::Beamline>(RAYX::importBeamline(rmlPath)); }