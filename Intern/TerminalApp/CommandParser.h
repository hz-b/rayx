#pragma once
#include <CLI/CLI.hpp>

#include "Debug/Debug.h"
#include "TerminalAppConfig.h"

class CommandParser {
  public:
    // Default constructor
    CommandParser() = default;
    // Custom constructor
    CommandParser(int _argc, char* const* _argv);

    ~CommandParser();
    /**
     * @brief Set command restrictions here (for ex int intervals etc.)
     *
     */
    void analyzeCommands();

    std::shared_ptr<CLI::App> m_cli11;
    // CLI Arguments
    // Flags initialize to DISABLED
    // Set options in .cpp file
    struct Args {
        bool m_plotFlag = false;          // -p (Plot)
        bool m_csvFlag = false;           // -c (.csv Output)
        bool m_cpuFlag = false;           // -x (CPU Tracer)
        bool m_benchmark = false;         // -b (Benchmark)
        bool m_multiplePlots = false;     // -m (Multiple Plots)
        bool m_version = false;           // -v (Version)
        std::string m_providedFile = "";  // -i (Input)
        bool m_isFixSeed = false;         // -f (Fixed Seed)
        int m_seed = -1;                  // -s (Provided Seed)
        bool m_verbose = false;           // --verbose (Verbose)
    } m_args;

    inline void getVersion() const {
        RAYX_LOG << R"(
          
        ╔═══╗╔═══╗╔╗  ╔╗     ╔═╗╔═╗
        ║╔═╗║║╔═╗║║╚╗╔╝║     ╚╗╚╝╔╝
        ║╚═╝║║║ ║║╚╗╚╝╔╝      ╚╗╔╝ 
        ║╔╗╔╝║╚═╝║ ╚╗╔╝ ╔═══╗ ╔╝╚╗ 
        ║║║╚╗║╔═╗║  ║║  ╚═══╝╔╝╔╗╚╗
        ╚╝╚═╝╚╝ ╚╝  ╚╝       ╚═╝╚═╝ HZB 2023.
        )";
        RAYX_LOG << "\n\t RAY-X Terminal Application " << TERMINALAPP_VERSION_MAJOR << "." << TERMINALAPP_VERSION_MINOR << "."
                 << TERMINALAPP_VERSION_PATCH << "." << TERMINALAPP_VERSION_TWEAK << "\n \t GIT: " << GIT_REVISION
                 << "\n \t BUILD: " << BUILD_TIMESTAMP;
    };

  private:
    int m_cli11_return;
    enum OptionType { BOOL, INT, STRING, BOOL_STRING };
    struct Options {
        // CLI::Option cli11_option;
        const OptionType type;
        const char* full_name;
        const char* description;
        void* option_flag;
    };
    // Map short arg to its parameters
    // this can also be done with app.get_option()
    std::unordered_map<char, Options> m_ParserCommands = {
        {'c', {OptionType::BOOL, "ocsv", "Output stored as .csv file.", &(m_args.m_csvFlag)}},
        {'b', {OptionType::BOOL, "benchmark", "Benchmark application: (RML → Trace → Output)", &(m_args.m_benchmark)}},
        {'m', {OptionType::BOOL, "mult", "Multiple plots extension at output.", &(m_args.m_multiplePlots)}},
        {'p', {OptionType::BOOL, "plot", "Plot output footprints and histograms.", &(m_args.m_plotFlag)}},
        {'x', {OptionType::BOOL, "cpu", "Tracing on CPU", &(m_args.m_cpuFlag)}},
        {'i', {OptionType::STRING, "input", "Input RML File or Directory.", &(m_args.m_providedFile)}},
        {'v', {OptionType::BOOL, "version", "", &(m_args.m_version)}},
        {'f', {OptionType::BOOL, "", "Fix the seed to RAYX::FIXED_SEED (Uses default)", &(m_args.m_isFixSeed)}},
        {'s', {OptionType::INT, "seed", "Provided user seed", &(m_args.m_seed)}},
        {'V', {OptionType::BOOL, "verbose", "Dump more information", &(m_args.m_verbose)}},
    };
};
