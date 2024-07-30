#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Core.h"
#include "Shader/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

namespace RAYX {

void RAYX_API writeCSV(const BundleHistory&, const std::string& filename, const Format& format, int startEventID = 0);

// loadCSV only works for csv files created using FULL_FORMAT.
BundleHistory RAYX_API loadCSV(const std::string& filename);

class RAYX_API CsvWriter {
  public:
    CsvWriter(const std::filesystem::path& filepath, const Format& format = FULL_FORMAT, int startEventIndex = 0);

    void write(const DeviceTracer::BatchOutput& batch);

  private:
    std::ofstream m_file;
    Format m_format;
    int m_startEventIndex;

    void validate();
};

}  // namespace RAYX
