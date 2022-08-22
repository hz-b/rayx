#include "CommandParser.h"

// CommandParser::CommandParser()  {}

CommandParser::CommandParser(int _argc, char* const* _argv)
    : m_cli11{std::make_shared<CLI::App>("Terminal Application for RAY-X")} {
    for (const std::pair<char, Options> option : m_ParserCommands) {
        // Full name string
        std::string _name;
        _name += "-";
        _name += option.first; // Short argument
        _name += ",--";
        _name.append(option.second.full_name); // Full argument

        const OptionType _type = option.second.type;
        const std::string _description(option.second.description);
        if ( _type == OptionType::BOOL) {          
            m_cli11->add_flag(_name, *static_cast<bool*>(option.second.option_flag),
                                      _description);                        
        } else if (_type == OptionType::STRING) {
            m_cli11->add_option(_name, *static_cast<std::string*>(option.second.option_flag),
                                      _description);
        }
    }

    ///// Parse
    try {
        m_cli11->parse(_argc, _argv);
    } catch (const CLI::ParseError &e) {
        m_cli11_return = m_cli11->exit(e);
        if((e.get_name() == "CallForHelp") ||  (e.get_name() == "CallForAllHelp"))
            exit(1);
        else
            RAYX_D_ERR << "CLI ERROR" << m_cli11_return << " " << e.get_name();
    }
}

void CommandParser::analyzeCommands(){
    if(m_args.m_dummyFlag && (!m_args.m_providedFile.empty())){
        RAYX_ERR << "Dummy Beamline and RML Beamline cannot be loaded at the same time.";
    }
    if(m_args.m_multiplePlots && !(m_args.m_plotFlag)){
        RAYX_ERR << "Please use --mult only when using --plot.";
    }
}


CommandParser::~CommandParser() = default;
