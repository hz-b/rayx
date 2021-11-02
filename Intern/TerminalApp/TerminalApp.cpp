#include <memory>

#include "TerminalApp.h"

TerminalApp::TerminalApp()
{
}

TerminalApp::TerminalApp(int argc, char** argv) :
    m_argc(argc),
    m_argv(argv)
{
    RAYX_DEBUG(std::cout << "[Terminal]: TerminalApp created!" << std::endl);
}

TerminalApp::~TerminalApp()
{
    RAYX_DEBUG(std::cout << "[Terminal]: TerminalApp deleted!" << std::endl);
}

void TerminalApp::run()
{
    RAYX_DEBUG(std::cout << "[Terminal]: TerminalApp running..." << std::endl);

    if (m_argc <= 2) {
        if (m_argc == 2) {
            // load rml file
            m_Beamline = std::make_shared<RAYX::Beamline>(RAYX::Importer::importBeamline(m_argv[1]));
            m_Presenter = RAYX::Presenter(m_Beamline);
        }
        else {
            loadDummyBeamline();
        }
        m_Presenter.run(0.0, 0.0, 0.0);
    }
    // rzp params
    else if (m_argc >= 4) {
        if (m_argc == 5) {
            // load rml
            m_Presenter = RAYX::Presenter(m_Beamline);
        }
        else {
            loadDummyBeamline();
        }
        m_Presenter.run(std::stod(std::string(m_argv[1])),
            std::stod(std::string(m_argv[2])), std::stod(std::string(m_argv[3])));
    }
}
