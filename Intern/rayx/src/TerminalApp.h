#pragma once

#include <chrono>
#include <filesystem>

#include "Beamline/Beamline.h"
#include "CommandParser.h"
#include "Debug/Instrumentor.h"
#include "RaySoA.h"
#include "TerminalAppConfig.h"
#include "Tracer/Tracer.h"

class TerminalApp {
  public:
    TerminalApp(int argc, char** argv);
    ~TerminalApp();

    void run();

  private:
    void tracePath(const std::filesystem::path& path);
    void traceRml(const std::filesystem::path& path);
    void traceBeamline(const RAYX::Beamline& beamline, const std::filesystem::path& outputPath);

    /// write rays to file
    /// @returns the output filename (either .csv or .h5)
    std::filesystem::path exportRays(const RAYX::RaySoA& rays, bool isCSV, const std::filesystem::path&, const RAYX::RayAttrFlag attr);

    std::unique_ptr<RAYX::Tracer> m_tracer;
    CliArgs m_cliArgs;
};
