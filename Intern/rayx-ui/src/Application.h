#pragma once

#include <optional>

#include "CommandParser.h"
#include "GraphicsCore/Descriptors.h"
#include "GraphicsCore/Renderer.h"
#include "RayProcessing.h"
#include "RenderSystem/UIRenderSystem.h"
// TODO: This is also in Device Class
const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Window;
class Device;
class RenderObject;
struct Line;

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

  private:
    // --- Order matters ---
    Window m_Window;                ///< Application window
    CommandParser m_CommandParser;  ///< Command line parser
    Device m_Device;                ///< Vulkan device
    Renderer m_Renderer;            ///< Vulkan renderer

    // --- Order doesn't matter ---
    RAYX::BundleHistory m_rays;                                       ///< All rays loaded from file
    RAYX::Beamline m_Beamline;                                        ///< Beamline loaded from file
    std::shared_ptr<DescriptorPool> m_GlobalDescriptorPool{nullptr};  ///< General descriptor pool
    std::shared_ptr<DescriptorPool> m_TexturePool{nullptr};           ///< Descriptor pool for textures

    void loadBeamline(const std::string& rmlPath);
    void loadRays(const std::string& rmlPath);

    void updateObjects(const std::string& path, std::vector<RenderObject>& rObjects);
    void createRayCache(const std::string& path, BundleHistory& rayCache, UIRayInfo& rayInfo);
    void updateRays(const std::string& path, BundleHistory& rayCache, std::optional<RenderObject>& rayObj, std::vector<Line>& rays,
                    UIRayInfo& rayInfo);
};
