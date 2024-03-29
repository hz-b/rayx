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

struct UISimulationInfo {
    int startEventID = 0;  // unused right now
    int maxEvents = 0;     // unused right now
    int maxBatchSize = 100000;
    int tracer = 1;
    bool sequential = false;
    std::vector<std::string> availableDevices;
    int deviceIndex = 0;
    bool fixedSeed = false;
    int seed;

    UISimulationInfo(int startEventID, int maxEvents, int maxBatchSize, int tracer, bool sequential, const std::vector<std::string>& availableDevices,
                     int deviceIndex, bool fixedSeed = false, int seed = 0)
        : startEventID(startEventID),
          maxEvents(maxEvents),
          maxBatchSize(maxBatchSize),
          tracer(tracer),
          sequential(sequential),
          availableDevices(availableDevices),
          deviceIndex(deviceIndex),
          fixedSeed(fixedSeed),
          seed(seed) {}
};

struct UIParameters {
    CameraController& camController;
    std::filesystem::path rmlPath;
    bool rmlReady;
    bool h5Ready;
    float frameTime;
    UIRayInfo rayInfo;
    bool runSimulation;
    bool simulationSettingsReady;
    UISimulationInfo simulationInfo;

    UIParameters(CameraController& camController, const std::vector<std::string>& availableDevices)
        : camController(camController),
          rmlPath(""),
          rmlReady(false),
          h5Ready(false),
          frameTime(0.0f),
          rayInfo({false, false, false, false, 0, 0}),
          runSimulation(false),
          simulationSettingsReady(false),
          simulationInfo({
              0,
              0,
              100000,
              0,
              false,
              availableDevices,
              0,
          }) {}

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