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
    rayx::Beamline loadBeamline(const std::filesystem::path& filepath);
    rayx::Rays traceBeamline(const rayx::Beamline& beamline, const rayx::RayAttrMask attr);
    void validateEvents(const rayx::Rays& rays);

    /// write rays to file
    /// @returns the output filename (either .csv or .h5)
    std::filesystem::path exportRays(const std::filesystem::path& filepath, const std::vector<std::string>& objectNames, const rayx::Rays& rays,
                                     const rayx::RayAttrMask attr);

    std::unique_ptr<rayx::Tracer> m_tracer;
    CliArgs m_cliArgs;
};
