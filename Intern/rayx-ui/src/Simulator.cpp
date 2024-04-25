#include "Simulator.h"

#include "Random.h"
#include "Writer/CSVWriter.h"
#include "Writer/H5Writer.h"
#include "Writer/Writer.h"

// constructor
Simulator::Simulator() {
    m_seq = RAYX::Sequential::No;
    m_availableDevices.push_back("placeholder-device-1");
}

void Simulator::runSimulation() {
    if (!m_readyForSimulation) {
        RAYX_ERR << "Simulator is not ready for simulation!";
        return;
    }
    // Run rayx core
    if (!m_maxEvents) {
        m_maxEvents = static_cast<unsigned int>(m_Beamline.m_DesignElements.size() + 2);
    }

    auto rays = m_Tracer->trace(m_Beamline, m_seq, m_max_batch_size, 1, m_maxEvents, m_startEventID);

    // check max EventID
    unsigned int maxEventID = 0;
    bool notEnoughEvents = false;

    for (auto& ray : rays) {
        if (ray.size() > (maxEventID)) {
            maxEventID = static_cast<unsigned int>(ray.size()) + m_startEventID;
        }

        for (auto& event : ray) {
            if (event.m_eventType == RAYX::ETYPE_TOO_MANY_EVENTS) {
                notEnoughEvents = true;
            }
        }
    }

    if (notEnoughEvents) {
        RAYX_LOG << "Not enough events (" << m_maxEvents << ")! Consider increasing m_maxEvents.";
    }
    if (maxEventID == 0) {
        RAYX_LOG << "No events were recorded! If startEventID is set, it might need to be lowered.";
    } else if (maxEventID < m_maxEvents) {
        RAYX_LOG << "m_maxEvents is set to " << m_maxEvents << " but the maximum event ID is " << maxEventID << ". Consider setting m_maxEvents to "
                 << maxEventID << " to increase performance.";
    }

    // Export Rays to external data.
    std::string path = m_RMLPath.string();
    if (path.ends_with(".rml")) {
        path = path.substr(0, path.length() - 4);
    } else {
        RAYX_ERR << "Input file is not an *.rml file!";
    }

    Format fmt = formatFromString(defaultFormatString());

    std::vector<std::string> names;
    names.reserve(m_Beamline.m_DesignElements.size());

    for (const auto& designElement : m_Beamline.m_DesignElements) {
        names.push_back(designElement.getName());
    }

    path += ".h5";
#ifndef NO_H5
    writeH5(rays, path, fmt, names, m_startEventID);
#else
    writeCSV(rays, path, fmt, m_startEventID);
#endif
}

void Simulator::setSimulationParameters(const std::filesystem::path& RMLPath, const RAYX::Beamline& beamline,
                                        const UISimulationInfo& simulationInfo) {
    m_RMLPath = RMLPath;
    m_Beamline = std::move(beamline);
    m_max_batch_size = simulationInfo.maxBatchSize;
    if (simulationInfo.tracer == 0) {
        m_Tracer = std::make_unique<RAYX::Tracer>(RAYX::Tracer::Platform::Cpu);
    } else {
        m_Tracer = std::make_unique<RAYX::Tracer>(RAYX::Tracer::Platform::Gpu);
    }
    m_seq = simulationInfo.sequential ? RAYX::Sequential::Yes : RAYX::Sequential::No;
    m_deviceIndex = simulationInfo.deviceIndex;
    m_startEventID = simulationInfo.startEventID;
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

std::vector<std::string> Simulator::getAvailableDevices() { return m_availableDevices; }
