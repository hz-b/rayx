#pragma once

#include "PythonInterp.h"
#include "RayCore.h"

// Virtual python Environment Path
#ifdef WIN32  // Todo
#define VENV_PATH "./rayxvenv/bin/python3"
#else
#define VENV_PATH "./rayxvenv/bin/python3"
#endif

enum OptFlags { Disabled, Enabled };

class TerminalApp : public RAYX::Application {
  public:
    TerminalApp();
    TerminalApp(int argc, char** argv);
    ~TerminalApp();

    void run() override;

    const std::string& getProvidedFilePath() const { return providedFile; };

  private:
    char** m_argv;
    int m_argc;
    std::string providedFile;

    // CLI Arguments
    // Flags initialize to DISABLED
    struct Optargs {
        OptFlags m_plotFlag = OptFlags::Disabled;  // -p (Plot)
        OptFlags m_h5 = OptFlags::Disabled;        // -h (H5 Output)
        char* m_providedFile = NULL;               // -i (Input)

    } m_optargs;
};