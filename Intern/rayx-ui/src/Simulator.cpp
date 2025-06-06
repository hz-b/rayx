#include "Simulator.h"

#include "Random.h"
#include "Writer/CSVWriter.h"
#include "Writer/H5Writer.h"
#include "Writer/Writer.h"

// constructor
Simulator::Simulator() { m_seq = RAYX::Sequential::No; }

void Simulator::runSimulation() {
    if (!m_readyForSimulation) {
        RAYX_EXIT << "Simulator is not ready for simulation!";
        return;
    }
    // Run rayx core
    if (!m_maxEvents) {
        m_maxEvents = RAYX::Tracer::defaultMaxEvents(&m_Beamline);
    }

    constexpr int RECORD_ALL_ELEMENTS = -1;
    auto rays =
        m_Tracer->trace(m_Beamline, m_seq, m_max_batch_size, m_maxEvents, RECORD_ALL_ELEMENTS);  // TODO: implement recordElementIndex for GUI?

    bool notEnoughEvents = false;

    for (auto& ray : rays) {
        for (auto& event : ray) {
            if (event.m_eventType == RAYX::EventType::TooManyEvents) {
                notEnoughEvents = true;
            }
        }
    }

    if (notEnoughEvents) {
        RAYX_LOG << "Not enough events (" << m_maxEvents << ")! Consider increasing m_maxEvents.";
    }

    // Export Rays to external data.
    std::string path = m_RMLPath.string();
    if (path.ends_with(".rml")) {
        path = path.substr(0, path.length() - 4);
    } else {
        RAYX_EXIT << "Input file is not an *.rml file!";
    }

    Format fmt = formatFromString(defaultFormatString());

    std::vector<std::string> names;
    auto elements = m_Beamline.getElements();
    names.reserve(elements.size());

    for (const auto designElement : elements) {
        names.push_back(designElement->getName());
    }

    path += ".h5";
#ifndef NO_H5
    writeH5(rays, path, fmt, names);
#else
    writeCSV(rays, path, fmt);
#endif
}

void Simulator::setSimulationParameters(const std::filesystem::path& RMLPath, const RAYX::Beamline& beamline,
                                        const UISimulationInfo& simulationInfo) {
    const auto deviceAlreadyEnabled = m_deviceConfig.devices[simulationInfo.deviceIndex].enable;
    if (!deviceAlreadyEnabled) {
        m_deviceConfig.disableAllDevices().enableDeviceByIndex(simulationInfo.deviceIndex);
        m_Tracer = std::make_unique<RAYX::Tracer>(m_deviceConfig);
    }

    m_RMLPath = RMLPath;
    m_Beamline = std::move(*static_cast<RAYX::Beamline*>(beamline.clone().get()));
    m_max_batch_size = simulationInfo.maxBatchSize;
    m_seq = simulationInfo.sequential ? RAYX::Sequential::Yes : RAYX::Sequential::No;
    m_maxEvents = simulationInfo.maxEvents;
    if (simulationInfo.fixedSeed) {
        if (simulationInfo.seed != -1) {
            RAYX::fixSeed(simulationInfo.seed);
        } else
            RAYX::fixSeed(RAYX::FIXED_SEED);

    } else {
        RAYX::randomSeed();
    }
    m_readyForSimulation = true;
}

std::vector<std::string> Simulator::getAvailableDevices() {
    auto deviceNames = std::vector<std::string>();
    for (const RAYX::DeviceConfig::Device& device : m_deviceConfig.devices) deviceNames.push_back(device.name);
    return deviceNames;
}
