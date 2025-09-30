#pragma once

#include <chrono>
#include <filesystem>

#include "Beamline/Beamline.h"
#include "CommandParser.h"
#include "Debug/Instrumentor.h"
#include "Rays.h"
#include "TerminalAppConfig.h"
#include "Tracer/Tracer.h"

class TerminalApp {
  public:
    TerminalApp(int argc, char** argv);
    ~TerminalApp();

    void run();

  private:
    int tracePath(const std::filesystem::path& path);
    void traceRmlAndExportRays(const std::filesystem::path& path);
    RAYX::Beamline loadBeamline(const std::filesystem::path& filepath);
    RAYX::Rays traceBeamline(const RAYX::Beamline& beamline, const RAYX::RayAttrMask attr);
    void validateEvents(const RAYX::Rays& rays);

    /// write rays to file
    /// @returns the output filename (either .csv or .h5)
    std::filesystem::path exportRays(const std::filesystem::path& filepath, const std::vector<std::string>& objectNames, const RAYX::Rays& rays,
                                     const RAYX::RayAttrMask attr);

    std::unique_ptr<RAYX::Tracer> m_tracer;
    CliArgs m_cliArgs;
};
