#pragma once

#include <vulkan/vulkan.h>

#include <filesystem>
#include <memory>
#include <optional>

#include "Beamline/Beamline.h"
#include "Camera.h"
#include "Debug/Debug.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"

// TODO: Divide this into passed and returned parameters

/**
 * UI Parameters such as toggles, paths, etc.
 */
struct UIRayInfo {
    bool raysLoaded;
    bool displayRays;
    bool raysChanged;
    bool cacheChanged;
    bool renderAllRays;
    size_t amountOfRays;
    size_t maxAmountOfRays;
};

struct UISimulationInfo {
    uint32_t maxEvents = 0;
    uint32_t maxBatchSize = 100000;
    bool sequential = false;
    std::vector<std::string> availableDevices;
    uint32_t deviceIndex = 0;
    bool fixedSeed = false;
    int seed;

    UISimulationInfo(int maxEvents, int maxBatchSize, bool sequential, const std::vector<std::string>& availableDevices, int deviceIndex,
                     bool fixedSeed = false, int seed = 0)
        : maxEvents(maxEvents),
          maxBatchSize(maxBatchSize),
          sequential(sequential),
          availableDevices(availableDevices),
          deviceIndex(deviceIndex),
          fixedSeed(fixedSeed),
          seed(seed) {}
};

enum class SelectedType { None = -1, LightSource = 0, OpticalElement = 1, Group = 2 };

struct UIBeamlineInfo {
    RAYX::Beamline* beamline = nullptr;          // Beamline optional, lifetime managed by Application
    RAYX::BeamlineNode* selectedNode = nullptr;  // Selection optional, lifetime managed by Beamline
    bool elementsChanged = false;
};

struct UIParameters {
    VkExtent2D sceneExtent;
    VkDescriptorSet sceneDescriptorSet;
    CameraController& camController;
    std::filesystem::path rmlPath;
    bool rmlReady;
    bool h5Ready;
    bool isSceneWindowHovered = false;
    bool showH5NotExistPopup = false;
    float frameTime;
    UIRayInfo rayInfo;
    bool runSimulation;
    bool simulationSettingsReady;
    UISimulationInfo simulationInfo;
    UIBeamlineInfo beamlineInfo;

    UIParameters(CameraController& camController, const std::vector<std::string>& availableDevices)
        : sceneExtent({720, 480}),
          sceneDescriptorSet(VK_NULL_HANDLE),
          camController(camController),
          rmlPath(""),
          rmlReady(false),
          h5Ready(false),
          frameTime(0.0f),
          rayInfo({false, false, false, false, false, 0, 0}),
          runSimulation(false),
          simulationSettingsReady(false),
          simulationInfo({
              0,
              100000,
              false,
              availableDevices,
              0,
          }),
          beamlineInfo() {}

    void updatePath(const std::filesystem::path& path) {
        if (path.empty()) return;
        if (path == rmlPath) return;
        if (!std::filesystem::exists(path)) {
            RAYX_EXIT << "RML file does not exist: " << path.string();
        }
#ifdef NO_H5
        if (std::filesystem::exists(path.string().substr(0, path.string().size() - 4) + ".csv")) {
            h5Ready = true;
        }
#else
        if (std::filesystem::exists(path.string().substr(0, path.string().size() - 4) + ".h5")) {
            h5Ready = true;
        }
#endif
        rmlPath = path;
        rmlReady = true;
    }
};
