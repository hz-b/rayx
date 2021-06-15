#include "TerminalApp.h"
#include "Debug.h"

TerminalApp::TerminalApp()
{
}

TerminalApp::TerminalApp(int argc, char** argv)
{
    m_argc = argc;
    m_argv = argv;
    DEBUG(std::cout << "TerminalApp erstellt!" << std::endl);
}

TerminalApp::~TerminalApp()
{
    DEBUG(std::cout << "TerminalApp deleted!" << std::endl);
}

void TerminalApp::run()
{
    DEBUG(std::cout << "TerminalApp running..." << std::endl);
    if (m_argc >= 4) {
        m_tracerInterface.run(std::stod(std::string(m_argv[1])),
            std::stod(std::string(m_argv[2])), std::stod(std::string(m_argv[3])));
    }
    else {
        m_tracerInterface.run(0.0, 0.0, 0.0);
    }
}
