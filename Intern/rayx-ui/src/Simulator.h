#pragma once

#include "Beamline/Beamline.h"
#include "Tracer/Tracer.h"
#include "UserInterface/Settings.h"

class Simulator {
  public:
    Simulator();
    void runSimulation();
    void setSimulationParameters(const std::filesystem::path& RMLPath, const RAYX::Beamline& beamline, const UISimulationInfo& simulationInfo);
    std::vector<std::string> getAvailableDevices();

  private:
    uint32_t m_maxEvents = 0;

    std::filesystem::path m_RMLPath;  ///< Path to the RML file
    RAYX::Beamline m_Beamline;        ///< Beamline
    uint64_t m_max_batch_size = 100000;
    std::unique_ptr<RAYX::Tracer> m_Tracer;
    RAYX::Sequential m_seq = RAYX::Sequential::No;
    RAYX::DeviceConfig m_deviceConfig;  ///< List of available devices. Selection of device for tracing
    bool m_readyForSimulation = false;

    // after Simulation
    RAYX::BundleHistory m_rays;  ///< Ray cache
};
