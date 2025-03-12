#pragma once

#include <future>

#include "CommandParser.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "GraphicsCore/Descriptors.h"
#include "GraphicsCore/Renderer.h"
#include "RayProcessing.h"
#include "Scene.h"
#include "Simulator.h"
#include "UserInterface/UIHandler.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Window;
class Device;
class RenderObject;
class GridRenderSystem;
class RayRenderSystem;
class ObjectRenderSystem;
struct DesignElement;
struct DesignSource;

class Application {
  public:
    /**
     * @brief Constructs a new rayx-ui application.
     *
     * @param width The width of the application window.
     * @param height The height of the application window.
     * @param name The name of the application.
     */
    Application(uint32_t width, uint32_t height, const char* name, int argc, char** argv);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();
    void handleEvent(const SDL_Event* event);

  private:
    enum class State {
        Initializing,
        InitializeSimulation,
        Simulating,
        LoadingBeamline,
        LoadingRays,
        BuildingRays,
        PrepareElements,
        BuildingElements,
        Running,
        RunningWithoutScene
    } m_State{State::Initializing};

    // --- Order matters ---
    Window m_Window;                ///< Application window
    CommandParser m_CommandParser;  ///< Command line parser
    Device m_Device;                ///< Vulkan device
    Renderer m_Renderer;            ///< Vulkan renderer
    Simulator m_Simulator;          ///< Rayx core simulator

    // --- Order doesn't matter ---
    std::unique_ptr<Scene> m_Scene;  ///< Scene
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::shared_ptr<DescriptorSetLayout> m_globalSetLayout{nullptr};
    std::shared_ptr<DescriptorPool> m_GlobalDescriptorPool{nullptr};  ///< General descriptor pool
    std::shared_ptr<DescriptorSetLayout> m_textureSetLayout{nullptr};
    std::shared_ptr<DescriptorPool> m_TexturePool{nullptr};  ///< Descriptor pool for textures

    std::unique_ptr<GridRenderSystem> m_gridRenderSystem;
    std::unique_ptr<ObjectRenderSystem> m_objectRenderSystem;
    std::unique_ptr<RayRenderSystem> m_rayRenderSystem;
    std::vector<std::unique_ptr<Buffer>> m_uboBuffers;

    // Settings
    Camera m_Camera;                   ///< Camera
    CameraController m_CamController;  ///< Camera controller
    UIParameters m_UIParams;           ///< UI parameters
    UIHandler m_UIHandler;             ///< UI render system

    // Caching, Helpers, and other stuff
    std::filesystem::path m_RMLPath;                   ///< Path to the RML file
    std::unique_ptr<RAYX::Beamline> m_Beamline;        ///< Beamline
    RAYX::BundleHistory m_rays;                        ///< All rays
    std::vector<std::vector<RAYX::Ray>> m_sortedRays;  ///< Rays sorted by element
    bool m_buildElementsNeeded = true;
    bool m_buildTextureNeeded = true;
    VkExtent2D sceneExtent = {1920, 1080};  // TODO: why do we need this?

    // TODO: Should be in Scene
    std::future<void> m_beamlineFuture;
    std::future<void> m_raysFuture;
    std::future<void> m_buildRayCacheFuture;
    std::future<std::vector<Scene::RenderObjectInput>> m_getRObjInputsFuture;
    // TODO: Should be in Simulator
    std::future<void> m_simulationFuture;

    void init();

    void loadRays(const std::filesystem::path& rmlPath, const size_t numElements);
    void loadBeamline(const std::filesystem::path& rmlPath);
};
