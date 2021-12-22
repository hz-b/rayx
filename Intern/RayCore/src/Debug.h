#pragma once

//

/**
 *  Include this if you want macros for debugging.
 *
 *  Include stdlib before this header if you need it
 *
 */

#include <iostream>
#include <string>
#include <sstream>

// Memory leak detection (RAYX_NEW instead of new allows leaks to be detected)
#ifdef RAY_DEBUG_MODE
#ifdef RAYX_PLATFORM_WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#define RAYX_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define RAYX_NEW new
#endif

// Debug only code; use it as: DEBUG(<statement>);
#ifdef RAY_DEBUG_MODE
#define RAYX_DEBUG(x) (x)
#else
#define RAYX_DEBUG(x) \
    do {              \
    } while (0)
#endif

namespace RAYX {

constexpr int PREFIX_LEN = 30;

/**
 *
 * In the following we define
 * RAYX_LOG: prints to std::cout
 * RAYX_ERR: prints to std::cerr
 * ... and their debug-only variants RAYX_D_LOG, RAYX_D_ERR.
 *
 * example usage:
 * RAYX_LOG << "I am " << age << " years old";
 * */

/**
 * @param ERR       whether to use std::cerr or std::cout
 */
template <bool ERR>
struct Log {
    /**
     * @param filename  the file where the log occured
     * @param line      the linenumber in which the log occured
     * */
    Log(std::string filename, int line) {
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
        while (4 + line_string.size() + filename.size() + pad.size() < PREFIX_LEN) { pad += " "; }
        stream() << "[" << pad << filename << ":" << line_string << "] ";
    }

    ~Log() { stream() << std::endl; }

    std::ostream& stream() const {
        if (ERR) {
            return std::cerr;
        } else {
            return std::cout;
        }
    }

    template <typename T>
    Log& operator<<(T t) {
        stream() << t;
        return *this;
    }
};

struct IgnoreLog {
    template <typename T>
    IgnoreLog& operator<<(T) {
        return *this;
    }
};
}  // namespace RAYX

#define RAYX_LOG RAYX::Log<false>(__FILE__, __LINE__)
#define RAYX_ERR RAYX::Log<true>(__FILE__, __LINE__)

#ifdef RAY_DEBUG_MODE
#define RAYX_D_LOG RAYX_LOG
#define RAYX_D_ERR RAYX_ERR

#else
#define RAYX_D_LOG RAYX::IgnoreLog()
#define RAYX_D_ERR RAYX::IgnoreLog()
#endif