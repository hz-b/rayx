#pragma once

#include <Tracer/Tracer.h>
#include <UserInterface/Settings.h>
#include <Beamline/Beamline.h>

class Simulator {
  public:
    Simulator();
    void runSimulation();
    void setSimulationParameters(const std::filesystem::path& RMLPath, const RAYX::Beamline& beamline, const UISimulationInfo& simulationInfo);
    std::vector<std::string> getAvailableDevices();

  private:
    unsigned int m_startEventID = 0;
    unsigned int m_maxEvents = 0;

    std::filesystem::path m_RMLPath;  ///< Path to the RML file
    RAYX::Beamline m_Beamline;        ///< Beamline
    uint64_t m_max_batch_size = 100000;
    std::unique_ptr<RAYX::Tracer> m_Tracer;
    RAYX::Sequential m_seq = RAYX::Sequential::No;
    std::vector<std::string> m_availableDevices;  ///< List of available devices
    unsigned int m_deviceIndex = 0;               ///< Index of the selected device
    bool m_readyForSimulation = false;

    // after Simulation
    RAYX::BundleHistory m_rays;  ///< Ray cache
};
