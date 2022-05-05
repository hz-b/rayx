#pragma once

#include <fstream>

#include "Writer.hpp"

class CSVWriter : public Writer {
  public:
    CSVWriter();
    virtual ~CSVWriter();
    virtual void appendRays(const std::vector<double>& rays, size_t index);

  private:
    std::ofstream m_outputFile;
};