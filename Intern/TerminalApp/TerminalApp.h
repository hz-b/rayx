#pragma once

#include <getopt.h>

#include "PythonInterp.h"
#include "RayCore.h"
#include "TerminalAppConfig.h"

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

    inline void getHelp() const {
        RAYX_LOG << "\n\nRAY-X Terminal usage: "
                 << "TerminalApp [OPTION].. [FILE]\n\n"
                 << "Options:\n"
                 << "-p --plot\t Plot output footprints and histograms.\n"
                 << "-c --ocsv\t Output stored as .csv file.\n"
                 << "-i --input\t Input RML File Path.\n"
                 << "-d --dummy\t Run an in-house Beamline.\n"
                 << "-h --help\t Output this message.\n"
                 << "-b --benchmark\t Benchmark application:\n"
                 << "\t\t RML Parse → Trace → Output Storage\n"
                 << "-v --version\n";
    }

    inline void getVersion() const {
        RAYX_LOG << R"(
          
        ╔═══╗╔═══╗╔╗  ╔╗     ╔═╗╔═╗
        ║╔═╗║║╔═╗║║╚╗╔╝║     ╚╗╚╝╔╝
        ║╚═╝║║║ ║║╚╗╚╝╔╝      ╚╗╔╝ 
        ║╔╗╔╝║╚═╝║ ╚╗╔╝ ╔═══╗ ╔╝╚╗ 
        ║║║╚╗║╔═╗║  ║║  ╚═══╝╔╝╔╗╚╗
        ╚╝╚═╝╚╝ ╚╝  ╚╝       ╚═╝╚═╝ HZB 2022.
        )";
        RAYX_LOG << "\t RAY-X Terminal Application " << TERMINALAPP_VERSION_MAJOR
                 << "." << TERMINALAPP_VERSION_MINOR << "."
                 << TERMINALAPP_VERSION_PATCH << "."
                 << TERMINALAPP_VERSION_TWEAK
                 << "\n \t GIT: " << GIT_REVISION 
                 << "\n \t BUILD: " << BUILD_TIMESTAMP ; 
    }  // TODO: CMake config needed

  private:
    char** m_argv;
    int m_argc;
    std::string providedFile;
    std::chrono::_V2::system_clock::time_point m_start_time;

    // CLI Arguments
    // Flags initialize to DISABLED
    // Set options in .cpp file
    struct Optargs {
        OptFlags m_plotFlag = OptFlags::Disabled;   // -p (Plot)
        OptFlags m_csvFlag = OptFlags::Disabled;    // -c (.csv Output)
        OptFlags m_dummyFlag = OptFlags::Disabled;  // -d (Dummy Beamline)
        OptFlags m_benchmark = OptFlags::Disabled;  // -b (Benchmark)
        char* m_providedFile = NULL;                // -i (Input)

    } m_optargs;
};