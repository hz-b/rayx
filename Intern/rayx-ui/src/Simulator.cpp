#include "Simulator.h"

#include "Random.h"
#include "Writer/CsvWriter.h"
#include "Writer/H5Writer.h"

// constructor
Simulator::Simulator() { m_seq = rayx::Sequential::No; }

void Simulator::runSimulation() {
    if (!m_readyForSimulation) {
        RAYX_EXIT << "Simulator is not ready for simulation!";
        return;
    }
    // Run rayx core
    if (!m_maxEvents) { m_maxEvents = rayx::defaultMaxEvents(m_Beamline.numObjects()); }

    const auto rays       = m_Tracer->trace(m_Beamline, m_seq, rayx::ObjectMask::allElements(), rayx::RayAttrMask::All, static_cast<int>(m_maxEvents),
                                            static_cast<int>(m_max_batch_size));
    const auto bundleHist = convertRaysToBundleHistory(rays.copy(), m_Beamline.numSources());

    bool notEnoughEvents = false;

    for (auto& ray : bundleHist) {
        for (auto& event : ray) {
            if (event.m_eventType == rayx::EventType::TooManyEvents) { notEnoughEvents = true; }
        }
    }

    if (notEnoughEvents) { RAYX_LOG << "Not enough events (" << m_maxEvents << ")! Consider increasing m_maxEvents."; }

    // Export Rays to external data.
    std::string path = m_RMLPath.string();
    if (path.ends_with(".rml")) {
        path = path.substr(0, path.length() - 4);
    } else {
        RAYX_EXIT << "Input file is not an *.rml file!";
    }

    path += ".h5";
#ifndef NO_H5
    rayx::writeH5(path, m_Beamline.getObjectNames(), rays);
#else
    rayx::writeCsv(path, rays);
#endif
}

void Simulator::setSimulationParameters(const std::filesystem::path& RMLPath, const rayx::Beamline& beamline,
                                        const UISimulationInfo& simulationInfo) {
    const auto deviceAlreadyEnabled = m_deviceConfig.devices[simulationInfo.deviceIndex].enable;
    if (!deviceAlreadyEnabled) {
        m_deviceConfig.disableAllDevices().enableDeviceByIndex(simulationInfo.deviceIndex);
        m_Tracer = std::make_unique<rayx::Tracer>(m_deviceConfig);
    }

    m_RMLPath        = RMLPath;
    m_Beamline       = std::move(*static_cast<rayx::Beamline*>(beamline.clone().get()));
    m_max_batch_size = simulationInfo.maxBatchSize;
    m_seq            = simulationInfo.sequential ? rayx::Sequential::Yes : rayx::Sequential::No;
    m_maxEvents      = simulationInfo.maxEvents;
    if (simulationInfo.fixedSeed) {
        if (simulationInfo.seed != -1) {
            rayx::fixSeed(simulationInfo.seed);
        } else
            rayx::fixSeed(rayx::FIXED_SEED);
    } else {
        rayx::randomSeed();
    }
    m_readyForSimulation = true;
}

std::vector<std::string> Simulator::getAvailableDevices() {
    auto deviceNames = std::vector<std::string>();
    for (const rayx::DeviceConfig::Device& device : m_deviceConfig.devices) deviceNames.push_back(device.name);
    return deviceNames;
}
