#ifndef NO_H5

#include "H5Writer.h"

#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>
#include <string>

#include "Debug/Debug.h"

using uint = unsigned int;

int count(const RAYX::BundleHistory& hist) {
    int c = 0;
    for (auto& ray_hist : hist) {
        c += ray_hist.size();
    }
    return c;
}

std::vector<float> toDoubles(const RAYX::BundleHistory& hist, const Format& format) {
    std::vector<float> output;
    output.reserve(count(hist) * format.size());

    for (uint ray_id = 0; ray_id < hist.size(); ray_id++) {
        const RAYX::RayHistory& ray_hist = hist[ray_id];
        for (uint event_id = 0; event_id < ray_hist.size(); event_id++) {
            const RAYX::Event& event = ray_hist[event_id];
            for (uint i = 0; i < format.size(); i++) {
                float next = format[i].get_double(ray_id, event_id, event);
                output.push_back(next);
            }
        }
    }
    return output;
}

void writeH5(const RAYX::BundleHistory& hist, std::string filename, const Format& format, std::vector<std::string> elementNames) {
    HighFive::File file(filename, HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);

    auto doubles = toDoubles(hist, format);

    try {
        // write data
        auto dataspace = HighFive::DataSpace({doubles.size() / format.size(), format.size()});
        auto dataset = file.createDataSet<float>("rays", dataspace);
        auto ptr = (float*)doubles.data();
        dataset.write_raw(ptr);

        // write element names
        for (unsigned int i = 0; i < elementNames.size(); i++) {
            auto& e = elementNames[i];
            auto dataspace = HighFive::DataSpace({e.size()});
            auto dataset = file.createDataSet<char>(std::to_string(i), dataspace);
            auto ptr = e.c_str();
            dataset.write_raw(ptr);
        }
    } catch (HighFive::Exception& err) {
        RAYX_ERR << err.what();
    }
}

RAYX::BundleHistory fromDoubles(const std::vector<float>& doubles, const Format& format) {
    auto formatSize = format.size();  // Now 16 with the inclusion of Ray-ID and Snapshot-ID
    auto numRays = doubles.size() / formatSize;

    if (doubles.size() % formatSize != 0) {
        throw std::invalid_argument("Size of doubles does not match expected size based on format");
    }

    RAYX::BundleHistory bundleHist;
    bundleHist.reserve(numRays);
    size_t double_index = 0;
    RAYX::RayHistory rayHist;
    while (double_index < doubles.size()) {
        // Extract and ignore Ray-ID and Snapshot-ID
        float rayId = doubles[double_index++];
        float eventId = doubles[double_index++];

        if (eventId == 0) {
            bundleHist.push_back(rayHist);
            rayHist.clear();
        }

        // Extract data for ray
        glm::vec3 origin(doubles[double_index], doubles[double_index + 1], doubles[double_index + 2]);
        float eventType = doubles[double_index + 3];
        glm::vec3 direction(doubles[double_index + 4], doubles[double_index + 5], doubles[double_index + 6]);
        float energy = doubles[double_index + 7];
        glm::vec4 stokes(doubles[double_index + 8], doubles[double_index + 9], doubles[double_index + 10], doubles[double_index + 11]);
        float pathLength = doubles[double_index + 12];
        float order = doubles[double_index + 13];
        float lastElement = doubles[double_index + 14];

        RAYX::Ray ray = {
            origin, eventType, direction, energy, stokes, pathLength, order, lastElement, 0.0,
        };

        rayHist.push_back(ray);

        double_index += formatSize - 2;
    }

    // Remove first empty snapshot
    bundleHist.erase(bundleHist.begin());

    return bundleHist;
}

RAYX::BundleHistory raysFromH5(std::string filename, const Format& format) {
    RAYX::BundleHistory rays;
    HighFive::File file(filename, HighFive::File::ReadOnly);

    std::vector<float> doubles;

    try {
        // read data
        auto dataset = file.getDataSet("rays");
        auto dims = dataset.getSpace().getDimensions();
        doubles.resize(dims[0] * dims[1]);
        dataset.read(doubles.data());

        rays = fromDoubles(doubles, format);

    } catch (HighFive::Exception& err) {
        RAYX_ERR << err.what();
    }

    return rays;
}

#endif
