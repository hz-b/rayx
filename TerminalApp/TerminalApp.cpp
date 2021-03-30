#include "TerminalApp.h"

TerminalApp::TerminalApp()
    : m_argc(0), m_argv(nullptr), providedFile(nullptr)
{
}

TerminalApp::TerminalApp(int argc, char** argv)
    : m_argc(argc), m_argv(argv), providedFile("")
{
    if (argc > 1) {
        if (argv[2] == "-f") {
            providedFile = argv[3];
        }
        else {
            std::cout << "Given arguments could not be recognised and will be ignored." << std::endl;
        }
    }
}

TerminalApp::~TerminalApp()
{
}