#include "CommandParser.h"

// CommandParser::CommandParser()  {}

CommandParser::CommandParser(int ___argc, char* const* ___argv)
    : m_cli11{std::make_shared<CLI::App>("Terminal Application for RAY-X")} {
    for (const std::pair<char, Options>& option : m_ParserCommands) {
        // Full name string
        std::string _name = "";
        _name += "-";
        _name += option.first; // Short argument
        _name += ",--";
        _name.append(option.second.full_name); // Full argument

        const std::string _type(option.second.type);
        const std::string _description(option.second.description);

        if ( _type == "bool") {
            m_cli11.get()->add_flag(_name, *static_cast<bool*>(option.second.option_flag),
                                      _description);
        } else if (_type == "string") {
            m_cli11.get()->add_option(_name, *static_cast<std::string*>(option.second.option_flag),
                                      _description);
        }
    }


    try {
        m_cli11.get()->parse(___argc, ___argv);
    } catch (const CLI::ParseError &e) {
        m_cli11_return = m_cli11.get()->exit(e);
        //RAYX_D_ERR << "CLI ERROR " << m_cli11_return;
    }
}

CommandParser::~CommandParser() {}
