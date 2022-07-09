#pragma once

#include <vector>

class Writer {
  public:
    Writer() = default;
    virtual ~Writer() = default;
    virtual void appendRays(const std::vector<double>& rays, size_t index) = 0;
};

// these includes allow the user to just import Writer.hpp and still access
// both CSVWriter and H5Writer.
#include "CSVWriter.hpp"
#include "H5Writer.hpp"