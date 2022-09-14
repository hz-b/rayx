#ifndef CI  // highfive doesn't work in CI

#include "H5Writer.h"

#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>
#include <string>

#include "Debug.h"

void writeH5(std::vector<RAYX::Ray>& rays, std::string filename) {
    HighFive::File file(filename, HighFive::File::ReadWrite |
                                      HighFive::File::Create |
                                      HighFive::File::Truncate);

    const uint32_t numDoubles =
        rays.size() * sizeof(RAYX::Ray) / sizeof(double);

    try {
        auto dataspace = HighFive::DataSpace({numDoubles});
        auto dataset = file.createDataSet<double>("0", dataspace);
        double* ptr = (double*)rays.data();
        dataset.write_raw(ptr);
    } catch (HighFive::Exception& err) {
        RAYX_ERR << err.what();
    }
}

#endif
