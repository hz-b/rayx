#pragma once
#include <CLI/CLI.hpp>
#include <unordered_map>

#include "Debug/Debug.h"
#include "TerminalAppConfig.h"
#include "Writer/Writer.h"

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
    void analyzeCommands() const;

    std::shared_ptr<CLI::App> m_cli11;
    // CLI Arguments
    // Flags initialize to DISABLED
    // Set options in .cpp file
    struct Args {
        bool m_plotFlag = false;                       // -p (Plot)
        bool m_csvFlag = false;                        // -c (.csv Output)
        bool m_cpuFlag = false;                        // -x (Trace on CPU)
        bool m_gpuFlag = false;                        // -X (Trace on GPU)
        int m_deviceID = -1;                           // -d (Device)
        bool m_listDevices = false;                    // -l (List Devices)
        bool m_benchmark = false;                      // -b (Benchmark)
        bool m_version = false;                        // -v (Version)
        std::string m_providedFile = "";               // -i (Input)
        std::string m_outPath = "";                    // -o (Output)
        bool m_isFixSeed = false;                      // -f (Fixed Seed)
        int m_seed = -1;                               // -s (Provided Seed)
        int m_BatchSize = 0;                           // -b (Vk batch size )
        bool m_sequential = false;                     // -S (sequential tracing)
        bool m_verbose = false;                        // --verbose (Verbose)
        std::string m_format = defaultFormatString();  // --format
        int m_maxEvents = -1;                          // -m (max events)
        int m_recordElementIndex = -1;                 // -R --record-element (element index)
    } m_args;

    static inline void getVersion() {
        RAYX_LOG << R"(
        
      ╔═══╗╔═══╗╔╗  ╔╗╔═╗╔═╗
      ║╔═╗║║╔═╗║║╚╗╔╝║╚╗╚╝╔╝
      ║╚═╝║║║ ║║╚╗╚╝╔╝ ╚╗╔╝ 
      ║╔╗╔╝║╚═╝║ ╚╗╔╝  ╔╝╚╗ 
      ║║║╚╗║╔═╗║  ║║  ╔╝╔╗╚╗
      ╚╝╚═╝╚╝ ╚╝  ╚╝  ╚═╝╚═╝ HZB 2023.
      )";
        RAYX_LOG << "\n\t rayx terminal application " << TERMINALAPP_VERSION_MAJOR << "." << TERMINALAPP_VERSION_MINOR << "."
                 << TERMINALAPP_VERSION_PATCH << "\n \t GIT: " << GIT_REVISION << "\n \t BUILD: " << BUILD_TIMESTAMP;
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
        {'B', {OptionType::BOOL, "benchmark", "Benchmark application: (RML → Trace → Output)", &(m_args.m_benchmark)}},
        {'b', {OptionType::INT, "batch", "Batch size for tracing", &(m_args.m_BatchSize)}},
        {'p', {OptionType::BOOL, "plot", "Plot output footprints and histograms.", &(m_args.m_plotFlag)}},
        {'x', {OptionType::BOOL, "cpu", "Tracine on CPU", &(m_args.m_cpuFlag)}},
        {'X', {OptionType::BOOL, "gpu", "Tracine on GPU", &(m_args.m_gpuFlag)}},
        {'d', {OptionType::INT, "device", "Pick device via Device ID", &(m_args.m_deviceID)}},
        {'l', {OptionType::BOOL, "list", "List available devices", &(m_args.m_listDevices)}},
        {'i', {OptionType::STRING, "input", "Input RML File or Directory.", &(m_args.m_providedFile)}},
        {'o', {OptionType::STRING, "output", "Output path or filename", &(m_args.m_outPath)}},
        {'v', {OptionType::BOOL, "version", "", &(m_args.m_version)}},
        {'f', {OptionType::BOOL, "", "Fix the seed to RAYX::FIXED_SEED (Uses default)", &(m_args.m_isFixSeed)}},
        {'s', {OptionType::INT, "seed", "Provided user seed", &(m_args.m_seed)}},
        {'S', {OptionType::BOOL, "sequential", "trace sequentially", &(m_args.m_sequential)}},
        {'V', {OptionType::BOOL, "verbose", "Dump more information", &(m_args.m_verbose)}},
        {'F', {OptionType::STRING, "format", "Format output CSV / H5 data", &(m_args.m_format)}},
        {'m', {OptionType::INT, "maxEvents", "Maximum number of events per ray", &(m_args.m_maxEvents)}},
        {'R', {OptionType::INT, "record-element", "Record events only for a specifc element", &(m_args.m_recordElementIndex)}},
    };
};
