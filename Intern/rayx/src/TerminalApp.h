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
    const std::string& getProvidedFilePath() const { return providedFile; };

  private:
    /**
     * @brief Write Rays into output file
     * @return true
     * @return false
     */
    char** m_argv;
    int m_argc;

    /// if `path` is an RML file, it will trace this file.
    /// if `path` is a directory, it will call `tracePath(child)` for all
    /// children of that directory.
    void tracePath(const std::filesystem::path& path);
    // returns the output filename (either .csv or .h5)
    std::filesystem::path exportRays(const RAYX::RaySoA& rays, bool isCSV, const std::filesystem::path&, const RAYX::RayAttrFlag attr);

    std::string providedFile;
    std::unique_ptr<CommandParser> m_CommandParser;
    std::unique_ptr<RAYX::Tracer> m_Tracer;
    std::unique_ptr<RAYX::Beamline> m_Beamline;
};
