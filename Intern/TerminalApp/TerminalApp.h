#pragma once

#include <getopt.h>

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

    inline void getHelp() const {
        RAYX_LOG << "\nRAY-X known commands:\n"
                 << "-p --plot\t Plot output footprints and histograms.\n"
                 << "-c --ocsv\t Output stored as .csv file.\n"
                 << "-i --input\t Input RML File Path.\n"
                 << "-d --dummy\t Run an in-house Beamline.\n"
                 << "-h --help\t Output this message.\n"
                 << "-v --version\n";
    }

    inline void getVersion() const {
        RAYX_LOG << R"(

        ██████╗  █████╗ ██╗   ██╗    ██╗  ██╗    
        ██╔══██╗██╔══██╗╚██╗ ██╔╝    ╚██╗██╔╝    
        ██████╔╝███████║ ╚████╔╝      ╚███╔╝     
        ██╔══██╗██╔══██║  ╚██╔╝       ██╔██╗     
        ██║  ██║██║  ██║   ██║       ██╔╝ ██╗    
        ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝       ╚═╝  ╚═╝ HZB 2022.)";
        RAYX_LOG << "RAY-X X.X.X Build xxxXXX";
    }  // TODO: CMake config needed

  private:
    char** m_argv;
    int m_argc;
    std::string providedFile;

    // CLI Arguments
    // Flags initialize to DISABLED
    // Set options in .cpp file
    struct Optargs {
        OptFlags m_plotFlag = OptFlags::Disabled;   // -p (Plot)
        OptFlags m_csvFlag = OptFlags::Disabled;    // -c (.csv Output)
        OptFlags m_dummyFlag = OptFlags::Disabled;  // -d (Dummy Beamline)
        char* m_providedFile = NULL;                // -i (Input)

    } m_optargs;
};