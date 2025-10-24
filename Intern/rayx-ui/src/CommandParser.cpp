#include "CommandParser.h"

#include <cstring>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"

CommandParser::CommandParser(int _argc, char* const* _argv) : m_cli11{std::make_shared<CLI::App>("for rayx-ui CLI")} {
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
        } else if (_type == OptionType::BOOL_STRING) {  // Discarded
            m_cli11->add_flag(_name, *static_cast<bool*>(option.second.option_flag), _description);
        } else if (_type == OptionType::INT) {
            m_cli11->add_option(_name, *static_cast<int*>(option.second.option_flag), _description);
        } else if (_type == OptionType::OPTIONAL_INT) {
            m_cli11->add_option(_name, *static_cast<std::optional<int>*>(option.second.option_flag), _description);
        }
    }

    ///// Parse
    try {
        m_cli11->parse(_argc, _argv);
    } catch (const CLI::ParseError& e) {
        int result = m_cli11->exit(e);
        if ((e.get_name() == "CallForHelp") || (e.get_name() == "CallForAllHelp"))
            exit(1);
        else
            std::cout << "CLI ERROR" << result << " " << e.get_name();
        exit(1);
    }

    if (m_args.m_verbose) {
        rayx::setDebugVerbose(true);
        RAYX_VERB << "Verbose logging enabled.";
    }
    if (m_args.m_benchmark) {
        RAYX_VERB << "Starting in Benchmark Mode.\n";
        rayx::BENCH_FLAG = true;
    }
}
