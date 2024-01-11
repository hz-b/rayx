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
};