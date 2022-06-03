#ifndef CI  // highfive doesn't work in CI

#pragma once

#include <highfive/H5File.hpp>
#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>

#include "Writer.hpp"

#define RAY_DOUBLE_COUNT 16 // Same as in RayList.h

class H5Writer : public Writer {
  public:
    H5Writer();
    virtual ~H5Writer() = default;
    virtual void appendRays(const std::vector<double>& rays, size_t index);

  private:
    HighFive::File m_file;
};

#endif