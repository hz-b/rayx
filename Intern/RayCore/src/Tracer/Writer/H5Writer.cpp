#ifndef CI  // highfive doesn't work in CI

#include "H5Writer.hpp"

#include <assert.h>

#include <sstream>
#include <string>

#include "Debug.h"



H5Writer::H5Writer()
    : m_file("output.h5", HighFive::File::ReadWrite | HighFive::File::Create |
                              HighFive::File::Truncate) {}

void H5Writer::appendRays(const std::vector<double>& outputRays, size_t index) {
    auto rows = outputRays.size() / RAY_DOUBLE_COUNT;
    assert((double)rows == rows);
    try {
        std::vector<size_t> dims{rows, 16};  // Max size of one chunk is 128MB
        std::stringstream ss;
        ss << index;
        std::string s;
        ss >> s;
        auto dataset =
            m_file.createDataSet<double>(s, HighFive::DataSpace(dims));
        dataset.write_raw(outputRays.data());
    } catch (HighFive::Exception& err) {
        RAYX_D_ERR << err.what();
    }
}

#endif