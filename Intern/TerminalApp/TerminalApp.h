#pragma once

#include <getopt.h>

#include <chrono>

#include "PythonInterp.h"
#include "CommandParser.h"
#include "RayCore.h"
#include "TerminalAppConfig.h"

// Virtual python Environment Path
#ifdef WIN32  // Todo
#define VENV_PATH "./rayxvenv/bin/python3"
#else
#define VENV_PATH "./rayxvenv/bin/python3"
#endif

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
    std::unique_ptr<CommandParser> m_CommandParser;
};
