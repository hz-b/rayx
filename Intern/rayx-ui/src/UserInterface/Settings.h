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
    bool pathChanged;
    float frameTime;
    UIRayInfo rayInfo;
    bool showRMLNotExistPopup;
    bool showH5NotExistPopup;
    bool pathValidState;

    UIParameters(CameraController& camController)
        : camController(camController),
          rmlPath(""),
          pathChanged(false),
          frameTime(0.0f),
          rayInfo({false, false, false, false, 0, 0}),
          showRMLNotExistPopup(false),
          showH5NotExistPopup(false),
          pathValidState(false) {}

    void updatePath(const std::filesystem::path& path) {
        if (path.empty()) return;
        if (path == rmlPath) return;
        rmlPath = path;
        pathChanged = true;
    }
};