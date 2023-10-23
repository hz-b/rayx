#pragma once
#include <CLI/CLI.hpp>
#include <unordered_map>

class CommandParser {
  public:
    // Default constructor
    CommandParser() = default;
    // Custom constructor
    CommandParser(int _argc, char* const* _argv);

    ~CommandParser() = default;

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
        std::string m_providedFile;  // -i (Input)
    } m_args;

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
        {'i', {OptionType::STRING, "input", "Input RML File or Directory.", &(m_args.m_providedFile)}},
    };
};
