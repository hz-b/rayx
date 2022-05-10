#include "Debug.h"

namespace RAYX {
constexpr int PREFIX_LEN = 30;

/**
 * @param filename  the file where the log occured
 * @param line      the linenumber in which the log occured
 * @param o         the stream to which to write to
 *
 * This function is only defined in this cpp file, and can not be included or
 * used anywhere else.
 * */

void formatDebugMsg(std::string filename, int line, std::ostream& o) {
    size_t idx = filename.find_last_of("/\\");
    if (idx != std::string::npos) {
        filename = filename.substr(idx + 1);
    }

    std::stringstream strm;
    strm << line;
    std::string line_string = strm.str();

    // this shortens filenames which are too long
    if (filename.size() + line_string.size() + 4 > PREFIX_LEN) {
        filename = filename.substr(0, PREFIX_LEN - 4 - line_string.size());
        filename[filename.size() - 1] = '.';
        filename[filename.size() - 2] = '.';
        filename[filename.size() - 3] = '.';
    }

    std::string pad;
    while (4 + line_string.size() + filename.size() + pad.size() < PREFIX_LEN) {
        pad += " ";
    }
    o << "[" << pad << filename << ":" << line_string << "] ";
}

Log::Log(std::string filename, int line) {
    formatDebugMsg(filename, line, std::cout);
}

Log::~Log() { std::cout << std::endl; }

Warn::Warn(std::string filename, int line) {
    std::cerr << "\x1B[31m";  // color red
    formatDebugMsg(filename, line, std::cerr);
}

Warn::~Warn() {
    std::cerr << std::endl << "\033[0m";  // color reset
}

Err::Err(std::string filename, int line) : filename(filename), line(line) {
    std::cerr << "\x1B[31m";  // color red
    formatDebugMsg(filename, line, std::cerr);
}

Err::~Err() {
    std::cerr << "\n";
    formatDebugMsg(filename, line, std::cerr);
    std::cerr << "Terminating...\033[0m" << std::endl;  // color reset
    exit(1);
}

const int PREC = 17;

void dbg(std::string filename, int line, std::string name,
         std::vector<double> v) {
    RAYX::Log(filename, line) << name << ":";

    int counter = 0;  // stores the number of elements in the stringstream
    std::stringstream s;
    for (size_t i = 0; i < v.size(); i++) {
        if (counter != 0) {
            s << " ";
        }
        s << std::setprecision(PREC) << v[i];

        counter++;
        if (counter == 4 &&
            v.size() == 16) {  // 4x4 things should be written in 4 rows
            counter = 0;
            RAYX::Log(filename, line) << s.str();
            s = std::stringstream();
        }
    }
    if (counter > 0) {
        RAYX::Log(filename, line) << s.str();
    }
}

}  // namespace RAYX
