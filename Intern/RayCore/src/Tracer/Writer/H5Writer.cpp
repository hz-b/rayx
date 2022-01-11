#include "H5Writer.hpp"

H5Writer::H5Writer() : m_file("output.h5", H5Easy::File::Overwrite) {}

void H5Writer::appendRays(const std::vector<double>& outputRays, size_t index) {
    std::string s = "/data/" + index;

    H5Easy::dump(m_file, s, outputRays);
}