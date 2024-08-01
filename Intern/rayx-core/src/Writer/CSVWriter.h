#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Core.h"
#include "Shader/Ray.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"
#include "Tracer/Scheduler.h"

namespace RAYX {

// loadCSV only works for csv files created using FULL_FORMAT.
BundleHistory RAYX_API loadCSV(const std::string& filename);

class RAYX_API Writer {
    public:
    virtual ~Writer() = 0;

    virtual void writeBatch(const Batch& batch) = 0;

    void writeBeamline(Scheduler::TraceResult&& traceResult) {
        for (auto& batchFuture : traceResult) {
            assert(batchFuture.valid());
            batchFuture.wait();
            const auto batch = batchFuture.get();
            writeBatch(batch);
        }
    }
};

class RAYX_API CsvWriter : public Writer {
  public:
    static constexpr int DEFAULT_PRECISION = 20;

    CsvWriter(const std::filesystem::path& filepath, const Format& format = FULL_FORMAT, int startEventIndex = 0, int precision = DEFAULT_PRECISION);

    void writeBatch(const Batch& batch) override;

  private:
    std::ofstream m_file;
    Format m_format;
    int m_startEventIndex;
    int m_precision;

    void validate();
};

}  // namespace RAYX
