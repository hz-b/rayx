#ifndef NO_H5

#include "H5Writer.h"

#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>
#include <string>

#include "Debug/Debug.h"

using uint = unsigned int;

int count(const RAYX::Rays& rays) {
    int c = 0;
    for (auto& r : rays) {
        c += r.size();
    }
    return c;
}

std::vector<double> toDoubles(const RAYX::Rays& rays, const Format& format) {
    std::vector<double> output;
    output.reserve(count(rays) * format.size());

    for (uint ray_id = 0; ray_id < rays.size(); ray_id++) {
        auto& snapshots = rays[ray_id];
        for (uint snapshot_id = 0; snapshot_id < snapshots.size(); snapshot_id++) {
            auto& ray = snapshots[snapshot_id];
            for (uint i = 0; i < format.size(); i++) {
                double next = format[i].get_double(ray_id, snapshot_id, ray);
                output.push_back(next);
            }
        }
    }
    return output;
}

RAYX::Rays fromDoubles(const std::vector<double>& doubles, const Format& format) {
    unsigned int formatSize = format.size();  // Now 16 with the inclusion of Ray-ID and Snapshot-ID
    unsigned int numRays = doubles.size() / formatSize;

    if (doubles.size() % formatSize != 0) {
        throw std::invalid_argument("Size of doubles does not match expected size based on format");
    }

    RAYX::Rays rays;
    rays.reserve(numRays);
    unsigned int double_index = 0;
    while (double_index < doubles.size()) {
        RAYX::Snapshots snapshots;

        for (unsigned int snapshot_id = 0; snapshot_id < formatSize; snapshot_id += formatSize) {
            // Extract and ignore Ray-ID and Snapshot-ID
            double_index++;  // ray id
            double_index++;  // snapshot id

            // Extract data for ray
            glm::dvec3 origin(doubles[double_index], doubles[double_index + 1], doubles[double_index + 2]);
            double weight = doubles[double_index + 3];
            glm::dvec3 direction(doubles[double_index + 4], doubles[double_index + 5], doubles[double_index + 6]);
            double energy = doubles[double_index + 7];
            glm::dvec4 stokes(doubles[double_index + 8], doubles[double_index + 9], doubles[double_index + 10], doubles[double_index + 11]);
            double pathLength = doubles[double_index + 12];
            double order = doubles[double_index + 13];
            double lastElement = doubles[double_index + 14];
            double extraParam = doubles[double_index + 15];

            RAYX::Ray ray = RAYX::Ray::makeRayFrom(origin, direction, stokes, energy, weight, pathLength, order, lastElement, extraParam);
            snapshots.push_back(ray);

            double_index += formatSize - 2;
        }

        rays.push_back(snapshots);
    }

    return rays;
}

void writeH5(const RAYX::Rays& rays, std::string filename, const Format& format, std::vector<std::string> elementNames) {
    HighFive::File file(filename, HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);

    auto doubles = toDoubles(rays, format);

    try {
        // write data
        auto dataspace = HighFive::DataSpace({doubles.size() / format.size(), format.size()});
        auto dataset = file.createDataSet<double>("rays", dataspace);
        auto ptr = (double*)doubles.data();
        dataset.write_raw(ptr);

        // write element names
        for (unsigned int i = 0; i < elementNames.size(); i++) {
            auto& e = elementNames[i];
            auto dataspace = HighFive::DataSpace({e.size()});
            auto dataset = file.createDataSet<char>(std::to_string(i + 1), dataspace);
            auto ptr = e.c_str();
            dataset.write_raw(ptr);
        }
    } catch (HighFive::Exception& err) {
        RAYX_ERR << err.what();
    }
}

void readH5(RAYX::Rays& rays, std::string filename, const Format& format, std::vector<std::string>& elementNames) {
    HighFive::File file(filename, HighFive::File::ReadOnly);

    std::vector<double> doubles;

    try {
        // read data
        auto dataset = file.getDataSet("rays");
        auto dims = dataset.getSpace().getDimensions();
        doubles.resize(dims[0] * dims[1]);
        dataset.read(doubles.data());

        rays = fromDoubles(doubles, format);  // assuming fromDoubles function exists to convert back to RAYX::Rays

        // read element names
        // int i = 0;
        // while (true) {
        //     try {
        //         std::vector<char> buffer;
        //         auto dataset = file.getDataSet(std::to_string(i + 1));
        //         auto dims = dataset.getSpace().getDimensions();
        //         buffer.resize(dims[0]);
        //         dataset.read(buffer.data());

        //         // Convert back to string and add to elementNames
        //         std::string elementName(buffer.begin(), buffer.end());
        //         elementNames.push_back(elementName);

        //         i++;
        //     } catch (HighFive::Exception& err) {
        //         // No more datasets to read
        //         break;
        //     }
        // }

    } catch (HighFive::Exception& err) {
        RAYX_ERR << err.what();
    }
}

#endif
