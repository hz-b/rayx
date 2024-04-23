#pragma once

#include <optional>

#include "CommandParser.h"
#include "GraphicsCore/Descriptors.h"
#include "GraphicsCore/Renderer.h"
#include "RayProcessing.h"
#include "UserInterface/UIHandler.h"
#include "Scene.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Window;
class Device;
class RenderObject;

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

    enum class State { Initializing, Loading, BuildingRays, BuildingElements, Running, RunningWithoutScene } m_State{State::Initializing};

  private:
    // --- Order matters ---
    Window m_Window;                ///< Application window
    CommandParser m_CommandParser;  ///< Command line parser
    Device m_Device;                ///< Vulkan device
    Renderer m_Renderer;            ///< Vulkan renderer

    // --- Order doesn't matter ---
    std::unique_ptr<Scene> m_Scene;                                   ///< Scene
    std::shared_ptr<DescriptorPool> m_GlobalDescriptorPool{nullptr};  ///< General descriptor pool
    std::shared_ptr<DescriptorPool> m_TexturePool{nullptr};           ///< Descriptor pool for textures

    Camera m_Camera;                   ///< Camera
    CameraController m_CamController;  ///< Camera controller
    UIParameters m_UIParams;           ///< UI parameters
    UIHandler m_UIHandler;             ///< UI render system

    std::filesystem::path m_RMLPath;             ///< Path to the RML file
    std::unique_ptr<RAYX::Beamline> m_Beamline;  ///< Beamline
    RAYX::BundleHistory m_rays;                  ///< Ray cache

    void init();

    void loadRays(const std::filesystem::path& rmlPath);
    void loadBeamline(const std::filesystem::path& rmlPath);

    void createRayCache(RAYX::BundleHistory& rayCache, UIRayInfo& rayInfo);
    void updateRays(RAYX::BundleHistory& rayCache, std::optional<RenderObject>& rayObj, UIRayInfo& rayInfo);
};
