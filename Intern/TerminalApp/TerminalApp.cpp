#include "TerminalApp.h"

#include "Debug.h"
#include "PathResolver.h"

// TODO: (potential) Replace Getopt with boost(header-only)
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
    const static struct option long_options[] = {
        {"plot", no_argument, 0, 'p'},
        {"input", required_argument, 0, 'i'},
        {"ocsv", no_argument, 0, 'c'},
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {"dummy", no_argument, 0, 'd'},
        {"benchmark", no_argument, 0, 'b'},
        {0, 0, 0, 0}};

    int c;
    int option_index;
    extern int opterr;
    opterr = 0;  // Set opt auto error output to silent

    while (
        (c = getopt_long(m_argc, m_argv,
                         "pi:cvhdb",  // : required, :: optional, 'none' nothing
                         long_options, &option_index)) != -1) {
        switch (c) {
            case '?':
                if (optopt == 'i')
                    RAYX_ERR << "Option -" << static_cast<char>(optopt)
                             << " needs an input RML file.\n";
                else if (isprint(optopt))
                    RAYX_ERR << "Unknown option -" << static_cast<char>(optopt)
                             << ".\n";
                else
                    RAYX_ERR << "Unknown option character. \n";
                getHelp();
                exit(1);
            case 'h':
                getHelp();
                exit(1);
            case 'v':
                getVersion();
                exit(1);
            case 'p':
                m_optargs.m_plotFlag = OptFlags::Enabled;
                break;
            case 'c':
                m_optargs.m_csvFlag = OptFlags::Enabled;
                break;
            case 'i':
                m_optargs.m_providedFile = optarg;
                break;
            case 'd':
                m_optargs.m_dummyFlag = OptFlags::Enabled;
                break;
            case 'b':
                m_optargs.m_benchmark = OptFlags::Enabled;
                break;
            case 0:
                RAYX_ERR << "No option given.";
                break;
            default:
                abort();
        }
    }

    auto start_time  = std::chrono::steady_clock::now();
    /////////////////// Argument treatement
    // Load RML files
    if (m_optargs.m_providedFile != NULL) {
        // load rml file
        m_Beamline = std::make_shared<RAYX::Beamline>(
            RAYX::importBeamline(m_optargs.m_providedFile));
        m_Presenter = RAYX::Presenter(m_Beamline);
    } else {
        // Benchmark mode
        if (m_optargs.m_benchmark) {
            RAYX_D_LOG << "Starting in Benchmark Mode.\n";
        }

        if (m_optargs.m_dummyFlag) {
            RAYX_D_LOG << "Loading dummy beamline.";
            loadDummyBeamline();
        } else {
            RAYX_LOG << "No Pipeline/Beamline provided, exiting..";
            exit(1);
        }
    }

    // Output File format
    if (m_optargs.m_csvFlag == OptFlags::Enabled) {
        RAYX_D_LOG << "CSV.\n";
        // TODO : Enhance writer
    }

    // Run RAY-X Core
    m_Presenter.run();

    if (m_optargs.m_benchmark) {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        RAYX_LOG << "Benchmark: Done in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time).count() << " ms" ;
    }

    //  Plot in Python
    if (m_optargs.m_plotFlag == OptFlags::Enabled) {
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
            if (m_optargs.m_providedFile){
                std::string _providedFile = m_optargs.m_providedFile;
                pyPlot->setPlotName( _providedFile.c_str());}
            pyPlot->execute();
        } catch (std::exception& e) {
            RAYX_ERR << e.what() << "\n";
        }
    }
}
