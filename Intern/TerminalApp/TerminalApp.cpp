#include "TerminalApp.h"

#include <Tracer/CpuTracer.h>
#include <Tracer/VulkanTracer.h>
#include <Writer/Writer.h>

#include <memory>
#include <stdexcept>

#include "Debug.h"
#include "PathResolver.h"

TerminalApp::TerminalApp() = default;

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
    // Check correct use (This will exit if error)
    m_CommandParser->analyzeCommands();

    auto start_time = std::chrono::steady_clock::now();
    /////////////////// Argument treatement
    if (m_CommandParser->m_args.m_version) {
        m_CommandParser->getVersion();
        exit(1);
    }
    // Load RML files
    if (!m_CommandParser->m_args.m_providedFile.empty()) {
        // load rml file
        m_Beamline = std::make_unique<RAYX::Beamline>(
            RAYX::importBeamline(m_CommandParser->m_args.m_providedFile));
    } else {
        RAYX_LOG << "No Pipeline/Beamline provided, exiting..";
        exit(1);
    }

    // Chose Hardware
    if (m_CommandParser->m_args.m_cpuFlag) {
        m_Tracer = std::make_unique<RAYX::CpuTracer>();
    } else {
        m_Tracer = std::make_unique<RAYX::VulkanTracer>();
    }

    if (m_CommandParser->m_args.m_benchmark) {
        RAYX_D_LOG << "Starting in Benchmark Mode.\n";
    }
    // Run RAY-X Core
    auto rays = m_Tracer->trace(*m_Beamline);

    // Export Rays to external data.
    exportRays(rays);
    #ifdef RAYX_DEBUG_MODE
    // Export Debug Matrics.
    exportDebug(rays.rayAmount());
    #endif

    if (m_CommandParser->m_args.m_benchmark) {
        std::chrono::steady_clock::time_point end =
            std::chrono::steady_clock::now();
        RAYX_LOG << "Benchmark: Done in "
                 << std::chrono::duration_cast<std::chrono::milliseconds>(
                        end - start_time)
                        .count()
                 << " ms";
    }

    //  Plot in Python
    if (m_CommandParser->m_args.m_plotFlag) {
        // Setup to create venv if needed
        try {
            std::shared_ptr<PythonInterp> pySetup =
                std::make_shared<PythonInterp>("py_setup", "setup",
                                               (const char*)nullptr);
            pySetup->execute();
        } catch (std::exception& e) {
            RAYX_ERR << e.what();
        }
        RAYX_D_LOG << "Python Setup OK.";

        // Call PythonInterp from rayx venv:
        // *Temporary method (Calls sys python interpreter that calls rayx
        // interpreter) [Python Dynamic linking problem]
        try {
            std::shared_ptr<PythonInterp> pyPlot =
                std::make_shared<PythonInterp>("py_plot_entry", "startPlot",
                                               (const char*)nullptr);
            if (!m_CommandParser->m_args.m_providedFile.empty()) {
                std::string _providedFile =
                    getFilename(m_CommandParser->m_args.m_providedFile);
                pyPlot->setPlotName(_providedFile.c_str());
            }
            if (m_CommandParser->m_args.m_dummyFlag) {
                pyPlot->setPlotName("Dummy Beamline");
            }
            if (m_CommandParser->m_args.m_multiplePlots) {
                pyPlot->setPlotType(3);
            }
            pyPlot->execute();
        } catch (std::exception& e) {
            RAYX_ERR << e.what();
        }
    }
}

void TerminalApp::exportRays(RAYX::RayList& rays) {
#ifdef CI
    bool csv = true;
#else
    bool csv = m_CommandParser->m_args.m_csvFlag;
#endif

    if (csv) {
        writeCSV(rays, "output.csv");
    } else {
#ifndef CI  // writeH5 is not defined in the CI!
        writeH5(rays, "output.h5");
#endif
    }
}

#ifdef RAYX_DEBUG_MODE
/**
 * @brief Gets All Debug Buffers and check if they are the identity matrix.
 * This is a default function to show how the implemented Debug Buffer works.
 * You can write your own checking func.
 *
 * @param Amount Amount of Debug Buffer(=Number of rays)
 */
void TerminalApp::exportDebug(std::size_t Amount) {
    struct debug_t {  // For Size reasonns
        glm::dmat4 mat;
    };
    auto d = m_Tracer->getDebugList();
    for (long unsigned int i = 0; i < Amount * sizeof(debug_t);
         i += sizeof(debug_t)) {
        auto* debugPtr = (struct debug_t*)(((uint8_t*)d) + i);
        if (isIdentMatrix(debugPtr->mat)) {
            RAYX_D_LOG << "@" << i / sizeof(debug_t);
            printDMat4(debugPtr->mat);
        }
    }
}
#endif
