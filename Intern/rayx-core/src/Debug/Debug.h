#pragma once

/// This file mostly exposes the RAYX_LOG, RAYX_VERB, RAYX_WARN and RAYX_ERR macros.
/// We mostly use these macros instead of std::cout / std::cerr for convenience reasons.
/// These macros automatically generate a newline at the end, and also print file and line number from where the print originated.

/// RAYX_LOG, RAYX_VERB prints to std::cout, whereas RAYX_WARN and RAYX_ERR are printed in red and go to std::cerr.
/// RAYX_VERB is used for "verbose" prints, they can be activated and deactivated using the setDebugVerbose function.
/// Finally RAYX_ERR is to be used for fatal errors, calling it will automatically terminate the program by calling the configurable `error_fn` defined below.
///
/// Each of these variants also has a "debug-only" variant, that gets deactivated in the release mode.
/// The "debug-only" variant is prefixed by "RAYX_D_" instead of "RAYX_".
/// For example the "debug-only" RAYX_LOG would be RAYX_D_LOG.

/// For quick debugging prints, this file further exposes RAYX_DBG(x).

#include <array>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// This include is necessary, as Debug implements a special formatting for Ray.
#include <Shader/Ray.h>
#include <Shader/Complex.h>

// Debug only code; use it as: DEBUG(<statement>);
#ifdef RAYX_DEBUG_MODE
#define RAYX_DEBUG(x) (x)
#else
#define RAYX_DEBUG(x) \
    do {              \
    } while (0)
#endif

#define STRING(s) #s

namespace RAYX {

///////////////////////////////////////////////
// LOGGING SYSTEM
///////////////////////////////////////////////

// activates / deactivates the printing of RAYX_VERB "verbose" prints.
void RAYX_API setDebugVerbose(bool);

// reads the "verbose" flag.
bool RAYX_API getDebugVerbose();

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

// The implementation of RAYX_LOG
struct RAYX_API Log {
    Log(std::string filename, int line);
    ~Log();

    template <typename T>
    Log& operator<<(T t) {
        std::cout << t;
        return *this;
    }
};

// The implementation of RAYX_WARN
struct RAYX_API Warn {
    Warn(std::string filename, int line);

    ~Warn();

    template <typename T>
    Warn& operator<<(T t) {
        std::cerr << t;
        return *this;
    }
};

// The implementation of RAYX_ERR
struct RAYX_API Err {
    std::string filename;
    int line;

    Err(const std::string& filename, int line);

    ~Err();

    template <typename T>
    Err& operator<<(T t) {
        std::cerr << t;
        return *this;
    }
};

// The implementation of RAYX_VERB
struct RAYX_API Verb {
    Verb(std::string filename, int line);
    ~Verb();

    template <typename T>
    Verb& operator<<(T t) {
        if (getDebugVerbose()) {
            std::cout << t;
        }
        return *this;
    }
};

// An empty implementation used in release when using "debug-only" prints like RAYX_D_LOG.
struct RAYX_API IgnoreLog {
    template <typename T>
    IgnoreLog& operator<<(T) {
        return *this;
    }
};

// the function to be called after RAYX_ERR happens.
// normally exit(1), but in the test suite it's ADD_FAILURE.
extern void RAYX_API (*error_fn)();

// Defines the actual RAYX logging macros using the structs defined above.
// The __FILE__ and __LINE__ macros contain the current filename and linenumber.
// They are defined for us by c++ https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
#define RAYX_LOG RAYX::Log(__FILE__, __LINE__)
#define RAYX_WARN RAYX::Warn(__FILE__, __LINE__)
#define RAYX_ERR RAYX::Err(__FILE__, __LINE__)
#define RAYX_VERB RAYX::Verb(__FILE__, __LINE__)

#ifdef RAYX_DEBUG_MODE
// In debug mode, RAYX_D_LOG is just the same as RAYX_LOG.
#define RAYX_D_LOG RAYX_LOG
#define RAYX_D_WARN RAYX_WARN
#define RAYX_D_ERR RAYX_ERR

#else
// In release mode, RAYX_D_LOG instead calls the IgnoreLog, hence discarding the print.
#define RAYX_D_LOG RAYX::IgnoreLog()
#define RAYX_D_WARN RAYX::IgnoreLog()
#define RAYX_D_ERR RAYX::IgnoreLog()
#endif

/////////////////////////////////////////////////////////////////////////////
// COLLECTION DEBUGGING SYSTEM
/////////////////////////////////////////////////////////////////////////////

/**
 *
 * In the following we define
 * RAYX_DBG: prints collection to RAYX_LOG for debugging
 *
 * example usage:
 * RAYX_DBG(orientation);
 * RAYX_DBG(position);
 * */

template <int N, int M>
inline std::vector<double> formatAsVec(glm::mat<N, M, double> arg) {
    std::vector<double> out(N * M);
    for (size_t i = 0; i < N * M; i++) {
        out[i] = arg[i / N][i % N];
    }
    return out;
}

template <int N>
inline std::vector<double> formatAsVec(glm::vec<N, double> arg) {
    std::vector<double> out(N);
    for (size_t i = 0; i < N; i++) {
        out[i] = arg[i];
    }
    return out;
}

template <size_t N>
inline std::vector<double> formatAsVec(std::array<double, N> arg) {
    std::vector<double> out(N);
    for (size_t i = 0; i < N; i++) {
        out[i] = arg[i];
    }
    return out;
}

inline std::vector<double> formatAsVec(double arg) { return {arg}; }

inline std::vector<double> formatAsVec(Ray arg) {
    return {arg.m_position.x,  arg.m_position.y,  arg.m_position.z, arg.m_eventType, arg.m_direction.x,
            arg.m_direction.y, arg.m_direction.z, arg.m_energy,     arg.m_stokes.x,  arg.m_stokes.y,
            arg.m_stokes.z,    arg.m_stokes.w,    arg.m_pathLength, arg.m_order,     arg.m_lastElement, arg.m_sourceID};
}

void dbg(const std::string& filename, int line, std::string name, std::vector<double> v);

#define RAYX_DBG(C) RAYX::dbg(__FILE__, __LINE__, #C, RAYX::formatAsVec(C))

} // namespace RAYX
