#ifndef NO_H5

#include "H5Writer.h"

#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>
#include <limits>
#include <string>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Shader/Ray.h"

// count the number of events from `hist`.
int count(const RAYX::BundleHistory& hist) {
    int c = 0;
    for (auto& ray_hist : hist) {
        c += ray_hist.size();
    }
    return c;
}

// Re-formats `hist` into a bunch of doubles using the format.
std::vector<double> toDoubles(const RAYX::BundleHistory& hist, const Format& format) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    std::vector<double> output;
    output.reserve(count(hist) * format.size());

    for (uint32_t ray_id = 0; ray_id < hist.size(); ray_id++) {
        const RAYX::RayHistory& ray_hist = hist[ray_id];
        for (uint32_t event_id = 0; event_id < ray_hist.size(); event_id++) {
            const RAYX::Ray& event = ray_hist[event_id];
            for (uint32_t i = 0; i < format.size(); i++) {
                double next = format[i].get_double(ray_id, event_id, event);
                output.push_back(next);
            }
        }
    }
    return output;
}

void writeH5(const RAYX::BundleHistory& hist, const std::string& filename, const Format& format, std::vector<std::string> elementNames) {
    HighFive::File file(filename, HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);

    auto doubles = toDoubles(hist, format);

    try {
        // write data
        auto dataspace = HighFive::DataSpace({doubles.size() / format.size(), format.size()});
        auto dataset = file.createDataSet<double>("rays", dataspace);
        auto ptr = (double*)doubles.data();
        dataset.write_raw(ptr);

        // write element names
        for (size_t i = 0; i < elementNames.size(); i++) {
            std::string& name = elementNames[i];
            dataspace = HighFive::DataSpace({name.size()});
            dataset = file.createDataSet<char>(std::to_string(i), dataspace);
            auto ptr = name.c_str();
            dataset.write_raw(ptr);
        }
    } catch (HighFive::Exception& err) {
        RAYX_EXIT << err.what();
    }
}

RAYX::BundleHistory fromDoubles(const std::vector<double>& doubles, const Format& format) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    const size_t formatSize = format.size();
    const size_t numEntries = doubles.size() / formatSize;

    if (doubles.size() % formatSize != 0) {
        throw std::invalid_argument("Size of doubles does not match expected size based on format");
    }

    RAYX::BundleHistory bundleHist;
    bundleHist.reserve(numEntries / 2);  // Estimate: assume at least 2 events per ray on average

    RAYX::RayHistory rayHist;
    rayHist.reserve(8);  // Estimate: assume 8 events per ray on average

    const double* data = doubles.data();

    size_t currentRayID = std::numeric_limits<size_t>::max();  // Initialize with an invalid Ray-ID

    for (size_t i = 0; i < numEntries; ++i) {
        const double* rayData = data + i * formatSize;

        size_t rayID = static_cast<size_t>(rayData[0]);  // Extract the Ray-ID

        if (rayID != currentRayID) {
            if (!rayHist.empty()) {
                bundleHist.push_back(std::move(rayHist));
                size_t lastRayHistSize = rayHist.size();
                rayHist.clear();
                rayHist.reserve(lastRayHistSize);
            }
            currentRayID = rayID;
        }

        const auto ray = RAYX::Ray{
            .m_position = {rayData[2], rayData[3], rayData[4]},       // origin
            .m_eventType = static_cast<RAYX::EventType>(rayData[5]),  // eventType
            .m_direction = {rayData[6], rayData[7], rayData[8]},      // direction
            .m_energy = rayData[9],                                   // energy
            .m_field =
                {
                    {rayData[10], rayData[11]},
                    {rayData[12], rayData[13]},
                    {rayData[14], rayData[15]},
                },
            .m_pathLength = rayData[16],   // pathLength
            .m_order = rayData[17],        // order
            .m_lastElement = rayData[18],  // lastElement
            .m_sourceID = rayData[19]      // sourceID
        };

        rayHist.push_back(ray);
    }

    if (!rayHist.empty()) {
        bundleHist.push_back(std::move(rayHist));
    }

    return bundleHist;
}

RAYX::BundleHistory raysFromH5(const std::string& filename, const Format& format) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX::BundleHistory rays;

    try {
        HighFive::File file(filename, HighFive::File::ReadOnly);

        std::vector<double> doubles;

        // read data
        auto dataset = file.getDataSet("rays");
        auto dims = dataset.getSpace().getDimensions();
        doubles.resize(dims[0] * dims[1]);
        dataset.read(doubles.data());
        if (doubles.size() == 0) {
            RAYX_WARN << "No rays found in " << filename;
            return rays;
        }
        rays = fromDoubles(doubles, format);
        RAYX_VERB << "Loaded " << rays.size() << " rays from " << filename;

    } catch (HighFive::Exception& err) {
        RAYX_EXIT << err.what();
    }
    return rays;
}

#endif
