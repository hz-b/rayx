#include "CommandParser.h"

CommandParser::CommandParser()  {}

CommandParser::CommandParser(int ___argc, char* const* ___argv) {
    int c;
    int option_index;
    extern int opterr;
    opterr = 0;  // Set opt auto error output to silent

    const struct option long_options[] = {{"plot", no_argument, 0, 'p'},
                                          {"input", required_argument, 0, 'i'},
                                          {"ocsv", no_argument, 0, 'c'},
                                          {"version", no_argument, 0, 'v'},
                                          {"help", no_argument, 0, 'h'},
                                          {"dummy", no_argument, 0, 'd'},
                                          {"benchmark", no_argument, 0, 'b'},
                                          {"multipleplot", no_argument, 0, 'm'},
                                          {0, 0, 0, 0}};                                      
    while ((c = getopt_long(
                ___argc, ___argv,
                "pi:cvhdbm",  // : required, :: optional, 'none' nothing
                long_options, &option_index)) != -1) {
        switch (c) {
            case '?':
                if (optopt == 'i')
                    RAYX_ERR << "Option -" << static_cast<char>(optopt)
                             << " needs an input RML file.\n";
                else if (isprint(optopt))
                    RAYX_ERR << "Unknown option -" << static_cast<char>(optopt)
                             << ".\n";
                else
                    RAYX_ERR << "Unknown option character. \n";
                getHelp();
                exit(1);
            case 'h':
                getHelp();
                exit(1);
            case 'v':
                getVersion();
                exit(1);
            case 'p':
                m_optargs.m_plotFlag = OptFlags::Enabled;
                break;
            case 'c':
                m_optargs.m_csvFlag = OptFlags::Enabled;
                break;
            case 'i':
                m_optargs.m_providedFile = optarg;
                break;
            case 'd':
                m_optargs.m_dummyFlag = OptFlags::Enabled;
                break;
            case 'b':
                m_optargs.m_benchmark = OptFlags::Enabled;
                break;
            case 'm':
                m_optargs.m_multiplePlots = OptFlags::Enabled;
                break;
            case 0:
                RAYX_ERR << "No option given.";
                break;
            default:
                abort();
        }
    }
}

CommandParser::~CommandParser(){}
