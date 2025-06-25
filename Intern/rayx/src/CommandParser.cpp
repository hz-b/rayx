#include "CommandParser.h"

#include <cstring>

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
