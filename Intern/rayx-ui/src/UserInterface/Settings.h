#pragma once

#include <filesystem>

#include "Camera.h"

/**
 * UI Parameters such as toggles, paths, etc.
 */
struct UIRayInfo {
    bool displayRays;
    bool raysChanged;
    bool cacheChanged;
    bool renderAllRays;
    size_t amountOfRays;
    size_t maxAmountOfRays;
};

struct UIParameters {
    CameraController& camController;
    std::filesystem::path rmlPath;
    bool rmlReady;
    bool h5Ready;
    float frameTime;
    UIRayInfo rayInfo;
    bool showRMLNotExistPopup;
    bool showH5NotExistPopup;
    bool pathValidState;
    bool runSimulation;
    bool simulationSettingsReady;

    UIParameters(CameraController& camController)
        : camController(camController),
          rmlPath(""),
          rmlReady(false),
          h5Ready(false),
          frameTime(0.0f),
          rayInfo({false, false, false, false, 0, 0}),
          showRMLNotExistPopup(false),
          showH5NotExistPopup(false),
          runSimulation(false),
          simulationSettingsReady(false),
          pathValidState(false) {}

    void updatePath(const std::filesystem::path& path) {
        if (path.empty()) return;
        if (path == rmlPath) return;
        if (!std::filesystem::exists(path)) {
            RAYX_ERR << "RML file does not exist: " << path.string();
        }
#ifdef NO_H5
        if (!std::filesystem::exists(path.string().substr(0, path.string().size() - 4) + ".csv")) {
            RAYX_ERR << "Matching CSV file for" << path.string() << "does not exist";
        }
#else
        if (!std::filesystem::exists(path.string().substr(0, path.string().size() - 4) + ".h5")) {
            RAYX_ERR << "Matching H5 file for" << path.string() << "does not exist";
        }
#endif
        rmlPath = path;
        rmlReady = true;
    }
};