#ifndef CI  // highfive doesn't work in CI

#include "H5Writer.h"

#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>
#include <string>

#include "Debug.h"

std::string intToString(int x) {
    std::stringstream ss;
    ss << x;
    std::string s;
    ss >> s;
    return s;
}

void writeH5(RAYX::RayList& rays, std::string filename) {
    HighFive::File file(filename, HighFive::File::ReadWrite |
                                      HighFive::File::Create |
                                      HighFive::File::Truncate);
    int counter = 0;

    for (auto l : rays.getData()) {
        try {
            std::vector<size_t> dims{l.size(),
                                     16};  // Max size of one chunk is 128MB
            auto name = intToString(counter);
            auto dataset =
                file.createDataSet<double>(name, HighFive::DataSpace(dims));
            auto* ptr = (double*)l.data();
            dataset.write_raw(ptr);
        } catch (HighFive::Exception& err) {
            RAYX_D_ERR << err.what();
        }

        counter += l.size();
    }
}

#endif
