#include "Debug/Debug.h"

#include <sstream>
#include <utility>

namespace RAYX {

// The length of filename and line number is 30 characters.
// If it doesn't find, we insert "...".
// This is used to have a visually unified output that is easier to read.
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
    o.precision(17);
    o << "[" << pad << filename << ":" << line_string << "] ";
}

Log::Log(std::string filename, int line) { formatDebugMsg(std::move(filename), line, std::cout); }

// The destructor of Log is used to finish the print by a newline.
Log::~Log() { std::cout << std::endl; }

Warn::Warn(std::string filename, int line) {
    // This is a so-called ANSI color code (or ANSI color sequence).
    // It changes the color of the following text, until we do a color-reset.
    std::cerr << "\x1B[31m";  // color red
    formatDebugMsg(std::move(filename), line, std::cerr);
}

Warn::~Warn() {
    std::cerr << std::endl << "\033[0m";  // color reset
}

Err::Err(const std::string& filename, int line) : filename(filename), line(line) {
    std::cerr << "\x1B[31m";  // color red
    formatDebugMsg(filename, line, std::cerr);
}

Err::~Err() {
    std::cerr << "\n";
    formatDebugMsg(filename, line, std::cerr);
    std::cerr << "Terminating...\033[0m" << std::endl;  // color reset

    error_fn();
}

Verb::Verb(std::string filename, int line) {
    // only print if the verbose flag it set!
    if (getDebugVerbose()) {
        formatDebugMsg(std::move(filename), line, std::cout);
    }
}

Verb::~Verb() {
    if (getDebugVerbose()) {
        std::cout << std::endl;
    }
}

// The default error_fn value. Exit with an error code of 1.
void exit1() { exit(1); }
void (*error_fn)() = exit1;

const int PREC = 17; // precision

// the implementation of RAYX_DBG.
// the std::vector<double> v contains the data of the object we intend to log.
void dbg(const std::string& filename, int line, std::string name, std::vector<double> v) {
    RAYX::Log(filename, line) << std::move(name) << ":";

    int counter = 0;  // stores the number of elements in the stringstream
    std::stringstream s;
    for (size_t i = 0; i < v.size(); i++) {
        if (counter != 0) {
            s << " ";
        }
        s << std::setprecision(PREC) << v[i];

        counter++;
        if (counter == 4 && v.size() == 16) {  // 4x4 things should be written in 4 rows
            counter = 0;
            RAYX::Log(filename, line) << s.str();
            s = std::stringstream();
        }
    }
    if (counter > 0) {
        RAYX::Log(filename, line) << s.str();
    }
}

// The verbose flag used for RAYX_VERB printing.
static bool VERBOSE = false;
void setDebugVerbose(bool b) { VERBOSE = b; }
bool getDebugVerbose() { return VERBOSE; }

}  // namespace RAYX
