#include "TerminalApp.h"

#include "Debug.h"

// TODO: (potential) Replace Getopt with boost(header-only)
#include <unistd.h>

#include <memory>
#include <stdexcept>

TerminalApp::TerminalApp() {}

TerminalApp::TerminalApp(int argc, char** argv) : m_argv(argv), m_argc(argc) {
    RAYX_D_LOG << "TerminalApp created!";
}

TerminalApp::~TerminalApp() { RAYX_D_LOG << "TerminalApp deleted!"; }

void TerminalApp::run() {
    RAYX_PROFILE_FUNCTION();

    RAYX_D_LOG << "TerminalApp running...";

    /////////////////// Argument Parser
    int c;
    while ((c = getopt(m_argc, m_argv, "pi:")) != -1) {
        switch (c) {
            case 'p':
                m_optargs.m_plotFlag = OptFlags::Enabled;
                break;
            case 'i':
                m_optargs.m_providedFile = optarg;
                break;
            case '?':
                if (optopt == 'i')
                    RAYX_ERR << "Option -" << static_cast<char>(optopt)
                             << " needs an input RML file.\n";
                else if (isprint(optopt))
                    RAYX_ERR << "Unknown option -" << static_cast<char>(optopt)
                             << ".\n\n"
                             << "Known commands:\n"
                             << "-p \t Plot output footprints and histograms.\n"
                             << "-i \t Input RML File Path.\n";
                else
                    RAYX_ERR << "Unknown option character. \n";
                break;
            default:
                abort();
        }
    }

    /////////////////// Argument treatement
    // Load RML files
    if (m_optargs.m_providedFile != NULL) {
        // load rml file
        m_Beamline = std::make_shared<RAYX::Beamline>(
            RAYX::importBeamline(m_optargs.m_providedFile));
        m_Presenter = RAYX::Presenter(m_Beamline);
    } else {
        RAYX_D_LOG << "test";
        loadDummyBeamline();
    }

    // Run RAY-X Core
    m_Presenter.run();
    m_optargs.m_plotFlag = OptFlags::Enabled;
    // Plotin Python
    if (m_optargs.m_plotFlag == OptFlags::Enabled) {
        // Setup to create venv if needed
        try {
            std::shared_ptr<PythonInterp> pySetup =
                std::make_shared<PythonInterp>("py_setup", "setup",
                                               (const char*)NULL);
            pySetup->execute();
        } catch (std::exception& e) {
            RAYX_ERR << e.what() << "\n";
        }

        // Call PythonInterp from rayx venv
        try {
            std::shared_ptr<PythonInterp> pyPlot =
                std::make_shared<PythonInterp>("py_plot", "plotOutput",
                                               VENV_PATH);
            pyPlot->setPlotFileName("output.h5");
            pyPlot->execute();
        } catch (std::exception& e) {
            RAYX_ERR << e.what() << "\n";
        }
    }
}
