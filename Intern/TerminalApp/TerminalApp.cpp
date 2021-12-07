#include "TerminalApp.h"

#include <memory>

TerminalApp::TerminalApp() {}

TerminalApp::TerminalApp(int argc, char** argv) : m_argv(argv), m_argc(argc) {
    RAYX_DEBUG(std::cout << "[Terminal]: TerminalApp created!" << std::endl);
}

TerminalApp::~TerminalApp() {
    RAYX_DEBUG(std::cout << "[Terminal]: TerminalApp deleted!" << std::endl);
}

void TerminalApp::run() {
    RAYX_D_LOG << "TerminalApp running...";

    if (m_argc == 2) {
        // load rml file
        m_Beamline = std::make_shared<RAYX::Beamline>(
            RAYX::Importer::importBeamline(m_argv[1]));
        m_Presenter = RAYX::Presenter(m_Beamline);
    } else {
        loadDummyBeamline();
    }
    m_Presenter.run();
}
