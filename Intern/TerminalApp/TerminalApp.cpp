#include "TerminalApp.h"

#include "Debug.h"
#include "PathResolver.h"

// TODO: (potential) Replace Getopt with boost(header-only)
#include <Tracer/CpuTracer.h>
#include <Tracer/VulkanTracer.h>
#include <Writer/Writer.h>
#include <unistd.h>

#include <memory>
#include <stdexcept>

TerminalApp::TerminalApp() {}

TerminalApp::TerminalApp(int argc, char** argv) : m_argv(argv), m_argc(argc) {
    initPathResolver(argv[0]);
    RAYX_D_LOG << "TerminalApp created!";
}

TerminalApp::~TerminalApp() { RAYX_D_LOG << "TerminalApp deleted!"; }

void TerminalApp::run() {
    RAYX_PROFILE_FUNCTION();

    RAYX_D_LOG << "TerminalApp running...";

    /////////////////// Argument Parser
    m_CommandParser = std::make_unique<CommandParser>(m_argc, m_argv);

    auto start_time = std::chrono::steady_clock::now();
    /////////////////// Argument treatement
    // Load RML files
    if (m_CommandParser->m_optargs.m_providedFile != NULL) {
        // load rml file
        m_Beamline = std::make_unique<RAYX::Beamline>(
            RAYX::importBeamline(m_CommandParser->m_optargs.m_providedFile));
    } else {
        RAYX_LOG << "No Pipeline/Beamline provided, exiting..";
        exit(1);
    }

    // Chose Hardware
    if (m_CommandParser->m_optargs.m_cpuFlag) {
        m_Tracer = std::make_unique<RAYX::CpuTracer>();
    } else {
        m_Tracer = std::make_unique<RAYX::VulkanTracer>();
    }

    // Run RAY-X Core
    auto rays = m_Tracer->trace(*m_Beamline);

    // Export Rays to external data.
    exportRays(rays);

    if (m_CommandParser->m_optargs.m_benchmark) {
        std::chrono::steady_clock::time_point end =
            std::chrono::steady_clock::now();
        RAYX_LOG << "Benchmark: Done in "
                 << std::chrono::duration_cast<std::chrono::milliseconds>(
                        end - start_time)
                        .count()
                 << " ms";
    }

    //  Plot in Python
    if (m_CommandParser->m_optargs.m_plotFlag ==
        CommandParser::OptFlags::Enabled) {
        // Setup to create genv if needed
        try {
            std::shared_ptr<PythonInterp> pySetup =
                std::make_shared<PythonInterp>("py_setup", "setup",
                                               (const char*)nullptr);
            pySetup->execute();
        } catch (std::exception& e) {
            RAYX_ERR << e.what() << "\n";
        }
        RAYX_D_LOG << "Python Setup OK.";

        // Call PythonInterp from rayx venv:
        // *Temporary method (Calls sys python interpreter that calls rayx
        // interpreter) [Python Dynamic linking problem]
        try {
            std::shared_ptr<PythonInterp> pyPlot =
                std::make_shared<PythonInterp>("py_plot_entry", "startPlot",
                                               (const char*)nullptr);
            if (m_CommandParser->m_optargs.m_providedFile) {
                std::string _providedFile =
                    m_CommandParser->m_optargs.m_providedFile;
                pyPlot->setPlotName(_providedFile.c_str());
            }
            if (m_CommandParser->m_optargs.m_multiplePlots ==
                CommandParser::OptFlags::Enabled) {
                pyPlot->setPlotType(3);
            }
            pyPlot->execute();
        } catch (std::exception& e) {
            RAYX_ERR << e.what() << "\n";
        }
    }
}

void TerminalApp::exportRays(RAYX::RayList& rays) {
#ifdef CI
    bool csv = true;
#else
    bool csv = m_CommandParser->m_optargs.m_csvFlag;
#endif

    if (csv) {
        writeCSV(rays, "output.csv");
    } else {
#ifndef CI  // writeH5 is not defined in the CI!
        writeH5(rays, "output.h5");
#endif
    }
}
