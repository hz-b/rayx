#include "Simulator.h"

#include "Random.h"
#include "Tracer/VulkanTracer.h"
#include "Writer/CSVWriter.h"
#include "Writer/H5Writer.h"
#include "Writer/Writer.h"

// constructor
Simulator::Simulator() {
    m_seq = RAYX::Sequential::No;
    std::vector<VkPhysicalDevice> deviceList;
    {
        RAYX::VulkanTracer VkTracer;
        deviceList = VkTracer.getPhysicalDevices();
    }
    for (const auto& device : deviceList) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        m_availableDevices.push_back(deviceProperties.deviceName);
    }
}

void Simulator::runSimulation() {
    if (!m_readyForSimulation) {
        RAYX_ERR << "Simulator is not ready for simulation!";
        return;
    }
    // Run rayx core
    m_maxEvents = static_cast<unsigned int>(m_Beamline.m_DesignElements.size() + 2);
    unsigned int startEventID = 0;

    auto rays = m_Tracer->trace(m_Beamline, m_seq, m_max_batch_size, 1, m_maxEvents, startEventID);

    // check max EventID
    unsigned int maxEventID = 0;
    bool notEnoughEvents = false;

    for (auto& ray : rays) {
        if (ray.size() > (maxEventID)) {
            maxEventID = static_cast<unsigned int>(ray.size()) + startEventID;
        }

        for (auto& event : ray) {
            if (event.m_eventType == ETYPE_TOO_MANY_EVENTS) {
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
    writeH5(rays, path, fmt, names, startEventID);
#else
    writeCSV(rays, path, fmt, startEventID);
#endif
}

void Simulator::setSimulationParameters(const std::filesystem::path& RMLPath, const RAYX::Beamline& beamline,
                                        const UISimulationInfo& simulationInfo) {
    m_RMLPath = RMLPath;
    m_Beamline = std::move(beamline);
    m_max_batch_size = simulationInfo.maxBatchSize;
    if (simulationInfo.tracer == 0) {
        m_Tracer = std::make_unique<RAYX::CpuTracer>();
    } else {
        m_Tracer = std::make_unique<RAYX::VulkanTracer>();
    }
    m_seq = simulationInfo.sequential ? RAYX::Sequential::Yes : RAYX::Sequential::No;
    m_deviceIndex = simulationInfo.deviceIndex;

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
