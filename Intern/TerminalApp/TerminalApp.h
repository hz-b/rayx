#pragma once

#include <Tracer/Tracer.h>

#include <filesystem>

#include "CommandParser.h"
#include "Plotter/Plotter.h"
#include "RayCore.h"
#include "TerminalAppConfig.h"

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
    void exportRays(const std::vector<RAYX::Ray>&, std::string);

#if defined(RAYX_DEBUG_MODE) && not defined(CPP)
    void exportDebug();
#endif
    std::string providedFile;
    std::unique_ptr<CommandParser> m_CommandParser;
    std::unique_ptr<RAYX::Tracer> m_Tracer;
    std::unique_ptr<RAYX::Beamline> m_Beamline;
};
