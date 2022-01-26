#pragma once

//

/**
 *  Include this if you want macros for debugging.
 *
 *  Include stdlib before this header if you need it
 *
 */

#include <iostream>
#include <sstream>
#include <string>

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
 * @param filename  the file where the log occured
 * @param line      the linenumber in which the log occured
 * @param o         the stream to which to write to
 * */
void formatDebugMsg(std::string filename, int line, std::ostream& o);

struct Log {
    Log(std::string filename, int line);
    ~Log();

    template <typename T>
    Log& operator<<(T t) {
        std::cout << t;
        return *this;
    }
};

struct Warn {
    Warn(std::string filename, int line);

    ~Warn();

    template <typename T>
    Warn& operator<<(T t) {
        std::cerr << t;
        return *this;
    }
};

struct Err {
    std::string filename;
    int line;

    Err(std::string filename, int line);

    ~Err();

    template <typename T>
    Err& operator<<(T t) {
        std::cerr << t;
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

#define RAYX_LOG RAYX::Log(__FILE__, __LINE__)
#define RAYX_WARN RAYX::Warn(__FILE__, __LINE__)
#define RAYX_ERR RAYX::Err(__FILE__, __LINE__)

#ifdef RAY_DEBUG_MODE
#define RAYX_D_LOG RAYX_LOG
#define RAYX_D_WARN RAYX_WARN
#define RAYX_D_ERR RAYX_ERR

#else
#define RAYX_D_LOG RAYX::IgnoreLog()
#define RAYX_D_WARN RAYX::IgnoreLog()
#define RAYX_D_ERR RAYX::IgnoreLog()
#endif