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

std::vector<double> toDoubles(const RAYX::BundleHistory& hist, const Format& format) {
    std::vector<double> output;
    output.reserve(count(hist) * format.size());

    for (uint ray_id = 0; ray_id < hist.size(); ray_id++) {
        const RAYX::RayHistory& ray_hist = hist[ray_id];
        for (uint event_id = 0; event_id < ray_hist.size(); event_id++) {
            const RAYX::Event& event = ray_hist[event_id];
            for (uint i = 0; i < format.size(); i++) {
                double next = format[i].get_double(ray_id, event_id, event);
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

#endif