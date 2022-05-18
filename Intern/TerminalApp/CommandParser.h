#pragma once
#include <getopt.h>

#include "Debug.h"
#include "TerminalAppConfig.h"

class CommandParser {
  public:
    // Default constructor
    CommandParser();
    // Custom constructor
    CommandParser(int ___argc, char* const* ___argv);

    ~CommandParser();

    enum OptFlags { Disabled, Enabled };

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
                 << "-m --mult. plot\t Multiple plots extension at output.\n"
                 << "-v --version\n";
    };

    // CLI Arguments
    // Flags initialize to DISABLED
    // Set options in .cpp file
    struct Optargs {
        OptFlags m_plotFlag = OptFlags::Disabled;       // -p (Plot)
        OptFlags m_csvFlag = OptFlags::Disabled;        // -c (.csv Output)
        OptFlags m_dummyFlag = OptFlags::Disabled;      // -d (Dummy Beamline)
        OptFlags m_benchmark = OptFlags::Disabled;      // -b (Benchmark)
        OptFlags m_multiplePlots = OptFlags::Disabled;  // -m (Multiple Plots)
        char* m_providedFile = NULL;                    // -i (Input)
    } m_optargs;

    inline void getVersion() const {
        RAYX_LOG << R"(
          
        ╔═══╗╔═══╗╔╗  ╔╗     ╔═╗╔═╗
        ║╔═╗║║╔═╗║║╚╗╔╝║     ╚╗╚╝╔╝
        ║╚═╝║║║ ║║╚╗╚╝╔╝      ╚╗╔╝ 
        ║╔╗╔╝║╚═╝║ ╚╗╔╝ ╔═══╗ ╔╝╚╗ 
        ║║║╚╗║╔═╗║  ║║  ╚═══╝╔╝╔╗╚╗
        ╚╝╚═╝╚╝ ╚╝  ╚╝       ╚═╝╚═╝ HZB 2022.
        )";
        RAYX_LOG << "\t RAY-X Terminal Application "
                 << TERMINALAPP_VERSION_MAJOR << "."
                 << TERMINALAPP_VERSION_MINOR << "."
                 << TERMINALAPP_VERSION_PATCH << "."
                 << TERMINALAPP_VERSION_TWEAK << "\n \t GIT: " << GIT_REVISION
                 << "\n \t BUILD: " << BUILD_TIMESTAMP;
    };  // TODO: CMake config needed

};
