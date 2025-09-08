#include "CommandParser.h"

#include <CLI/CLI.hpp>

#include "Random.h"
#include "TerminalAppConfig.h"

void getVersion() {
    RAYX_LOG << R"(

  ╔═══╗╔═══╗╔╗  ╔╗╔═╗╔═╗
  ║╔═╗║║╔═╗║║╚╗╔╝║╚╗╚╝╔╝
  ║╚═╝║║║ ║║╚╗╚╝╔╝ ╚╗╔╝
  ║╔╗╔╝║╚═╝║ ╚╗╔╝  ╔╝╚╗
  ║║║╚╗║╔═╗║  ║║  ╔╝╔╗╚╗
  ╚╝╚═╝╚╝ ╚╝  ╚╝  ╚═╝╚═╝ HZB 2023.
  )";
    RAYX_LOG << "\n\t rayx terminal application " << TERMINALAPP_VERSION_MAJOR << "." << TERMINALAPP_VERSION_MINOR << "." << TERMINALAPP_VERSION_PATCH
             << "\n \t GIT: " << GIT_REVISION << "\n \t BUILD: " << BUILD_TIMESTAMP;
};

inline CliArgs parseCliArgs(const int argc, char const* const* const argv) {
    CliArgs args;
    CLI::App app{"Terminal Application for RAYX"};

    app.add_flag("-p,--plot", args.plot, "Plot output footprints and histograms");
    app.add_flag("-c,--csv", args.csv, "Output stored as .csv file");
    app.add_flag("-x,--cpu", args.cpu, "Trace on CPU");
    app.add_flag("-X,--gpu", args.gpu, "Trace on GPU");
    app.add_flag("-l,--list-devices", args.listDevices, "List available devices");
    app.add_flag("-B,--benchmark", args.benchmark, "Benchmark application");
    app.add_flag("-v,--version", args.version, "Show version information");
    app.add_flag("-S,--sequential", args.sequential, "Trace sequentially");
    app.add_flag("-V,--verbose", args.verbose, "Dump more information");
    app.add_flag("-f,--default-seed", args.defaultSeed, std::format("Use default fixed seed: {}", FIXED_SEED));

    app.add_option("-m,--maxevents", args.maxEvents, "Maximum number of events per ray");
    app.add_option("-D,--dump", args.dump, "Dump the meta data of a file (h5 or rml)");
    app.add_option("-i,--input", args.inputPaths, "Input RML files or directories (traversed recursively, looking for RML files)");
    app.add_option("-o,--output", args.outputPath, "Output path or filename");
    app.add_option("-s,--seed", args.seed, "Use fixed seed");
    app.add_option("-b,--batch", args.batchSize, std::format("Batch size for tracing (default: {})", DEFAULT_BATCH_SIZE));
    app.add_option("-d,--device", args.deviceId, "Pick device via Device ID");
    app.add_option("-R,--record-indices", args.recordIndices,
                   "Record events only for specific sources / elements (use --dump to list the indices of beamline objects)");

    app.add_option("-F,--format", args.format, "Record only specific Ray attributes to the output H5 file"); // TODO list options, so the user knows

    // add position arguments to input paths
    std::vector<std::string> inputPaths;
    app.add_option("positional", inputPaths);
    args.inputPaths.insert(args.inputPaths.end(), inputPaths.begin(), inputPaths.end());

    CLI11_PARSE(app, argc, argv);

    // --- Handle version ---
    if (args.version) {
        getVersion();
        std::exit(0);
    }

    if (args.defaultSeed && args.seed)
        RAYX_EXIT << "Please do not provide '--default-seed' and '--seed' simultaneously'";

    // TODO: handle more edge cases

    return args;
}

// CommandParser::CommandParser()  {}

CommandParser::CommandParser(int _argc, char* const* _argv) : m_cli11{std::make_shared<CLI::App>("Terminal Application for rayx")} {
    for (const std::pair<char, Options> option : m_ParserCommands) {
        // Full name string
        std::string _name;
        _name += "-";
        _name += option.first;  // Short argument
        if (strcmp(option.second.full_name, "") != 0) {
            _name += ",--";
            _name.append(option.second.full_name);  // Full argument
        }

        const OptionType _type = option.second.type;
        const std::string _description(option.second.description);
        if (_type == OptionType::BOOL) {
            m_cli11->add_flag(_name, *static_cast<bool*>(option.second.option_flag), _description);
        } else if (_type == OptionType::STRING) {
            m_cli11->add_option(_name, *static_cast<std::string*>(option.second.option_flag), _description);
        } else if (_type == OptionType::INT) {
            m_cli11->add_option(_name, *static_cast<int*>(option.second.option_flag), _description);
        } else if (_type == OptionType::INT_VEC) {
            m_cli11->add_option(_name, *static_cast<std::vector<int>*>(option.second.option_flag), _description)
                ->expected(-1);  // allow any number of ints
        }
    }

    ///// Parse
    try {
        m_cli11->parse(_argc, _argv);
    } catch (const CLI::ParseError& e) {
        m_cli11_return = m_cli11->exit(e);
        if ((e.get_name() == "CallForHelp") || (e.get_name() == "CallForAllHelp"))
            exit(1);
        else
            RAYX_D_ERR << "CLI ERROR" << m_cli11_return << " " << e.get_name();
    }
}

void CommandParser::analyzeCommands() const {
    if (!m_args.m_isFixSeed && m_args.m_seed != -1) {
        RAYX_EXIT << "Cannot use user-defined seed without -f, try -f-seed <seed>";
    }

    if (m_args.m_isFixSeed && m_args.m_seed < -1) {
        RAYX_EXIT << "Unsupported seed <= 0";
    }
}

CommandParser::~CommandParser() = default;
