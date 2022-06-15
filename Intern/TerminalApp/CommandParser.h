#pragma once

#include <CLI/CLI.hpp>

#include "Debug.h"
#include "TerminalAppConfig.h"

class CommandParser {
  public:
    // Default constructor
    CommandParser() = default;
    // Custom constructor
    CommandParser(int ___argc, char* const* ___argv);

    ~CommandParser();

    enum OptFlags { Disabled, Enabled };
    std::shared_ptr<CLI::App> m_cli11;
    int m_cli11_return;

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
        bool m_plotFlag = false;       // -p (Plot)
        bool m_csvFlag = false;        // -c (.csv Output)
        bool m_dummyFlag = false;      // -d (Dummy Beamline)
        bool m_benchmark = false;      // -b (Benchmark)
        bool m_multiplePlots = false;  // -m (Multiple Plots)
        bool m_version = false;        // -v (Version)
        std::string m_providedFile = "";            // -i (Input)
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
  private:
    struct Options {
        // CLI::Option cli11_option;
        const char* type;
        const char* full_name;
        const char* description;
        void* option_flag;
    };

    // Map short arg to its parameters
    std::unordered_map<char, Options> m_ParserCommands = {
        {'p',
         {"bool", "plot", "Plot output footprints and histograms.",
          &(m_optargs.m_plotFlag)}},
        {'c',
         {"bool", "ocsv", "Output stored as .csv file.",
          &(m_optargs.m_csvFlag)}},
        {'d',
         {"bool", "dummy", "Run an in-house built Beamline.",
          &(m_optargs.m_dummyFlag)}},
        {'b',
         {"bool", "benchmark",
          "Benchmark application: \t (RML Parse → Trace → Output Storage)",
          &(m_optargs.m_benchmark)}},
        {'m',
         {"bool", "mult", "Multiple plots extension at output.",
          &(m_optargs.m_multiplePlots)}},
        {'i',
         {"string", "input", "Input RML File Path.",
          &(m_optargs.m_providedFile)}}
        // {'v', {"bool", "version", "", (void*)m_optargs.m_version}},
    };
};
