#include "Simulator.h"

#include "Tracer/VulkanTracer.h"
#include "Writer/H5Writer.h"

void Simulator::runSimulation() {
    // Run rayx core
    m_maxEvents = m_Beamline.m_OpticalElements.size() + 2;
    int startEventID = 0;

    auto rays = m_Tracer->trace(m_Beamline, m_seq, m_max_batch_size, 1, m_maxEvents, startEventID);

    // check max EventID
    unsigned int maxEventID = 0;
    bool notEnoughEvents = false;

    for (auto& ray : rays) {
        if (ray.size() > (maxEventID)) {
            maxEventID = ray.size() + startEventID;
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
    names.reserve(m_Beamline.m_OpticalElements.size());

    for (const auto& opticalElement : m_Beamline.m_OpticalElements) {
        names.push_back(opticalElement.m_name);
    }

    path += ".h5";
    writeH5(rays, path, fmt, names, startEventID);
}

void Simulator::setSimulationParameters(std::filesystem::path RMLPath, RAYX::Beamline beamline, uint64_t max_batch_size, int tracer,
                                        bool sequential) {
    m_RMLPath = RMLPath;
    m_Beamline = std::move(beamline);
    m_max_batch_size = max_batch_size;
    if (tracer == 0) {
        m_Tracer = std::make_unique<RAYX::CpuTracer>();
    } else {
        m_Tracer = std::make_unique<RAYX::VulkanTracer>();
    }
    m_seq = sequential ? RAYX::Sequential::Yes : RAYX::Sequential::No;
}
