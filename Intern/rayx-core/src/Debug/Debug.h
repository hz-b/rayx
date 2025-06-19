#pragma once

/// This file mostly exposes the RAYX_LOG, RAYX_VERB, RAYX_WARN and RAYX_EXIT macros.
/// We mostly use these macros instead of std::cout / std::cerr for convenience reasons.
/// These macros automatically generate a newline at the end, and also print file and line number from where the print originated.

/// RAYX_LOG, RAYX_VERB prints to std::cout, whereas RAYX_WARN and RAYX_EXIT are printed in red and go to std::cerr.
/// RAYX_VERB is used for "verbose" prints, they can be activated and deactivated using the setDebugVerbose function.
/// Finally RAYX_EXIT is to be used for fatal errors, calling it will automatically terminate the program by calling the configurable `error_fn`
/// defined below.
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
#include "RaySoA.h"
#include "Shader/Ray.h"

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

/////////////////////////////////////////////////////////////////////////////
// OSTREAM CONVERSION
/////////////////////////////////////////////////////////////////////////////

inline std::ostream& operator<<(std::ostream& os, const complex::Complex& c) { return os << "{" << c.real() << ", " << c.imag() << "}"; }

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const glm::tvec2<T>& v) {
    return os << "{" << v.x << ", " << v.y << "}";
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const glm::tvec3<T>& v) {
    return os << "{" << v.x << ", " << v.y << ", " << v.z << "}";
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const glm::tvec4<T>& v) {
    return os << "{" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "}";
}

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
 * RAYX_EXIT: prints to std::cerr
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

// The implementation of RAYX_EXIT
struct RAYX_API Exit {
    std::string filename;
    int line;

    Exit(const std::string& filename, int line);

    ~Exit();

    template <typename T>
    Exit& operator<<(T t) {
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
struct RAYX_API IgnoreLog{template <typename T> IgnoreLog & operator<<(T){return *this;
}  // namespace RAYX
}
;

// the function to be called after RAYX_EXIT happens.
// normally exit(1), but in the test suite it's ADD_FAILURE.
extern void RAYX_API (*error_fn)();

// Defines the actual RAYX logging macros using the structs defined above.
// The __FILE__ and __LINE__ macros contain the current filename and linenumber.
// They are defined for us by c++ https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
#define RAYX_LOG RAYX::Log(__FILE__, __LINE__)
#define RAYX_WARN RAYX::Warn(__FILE__, __LINE__)
#define RAYX_EXIT RAYX::Exit(__FILE__, __LINE__)
#define RAYX_VERB RAYX::Verb(__FILE__, __LINE__)

#ifdef RAYX_DEBUG_MODE
// In debug mode, RAYX_D_LOG is just the same as RAYX_LOG.
#define RAYX_D_LOG RAYX_LOG
#define RAYX_D_WARN RAYX_WARN
#define RAYX_D_ERR RAYX_EXIT
#define RAYX_D_VERB RAYX_VERB

#else
// In release mode, RAYX_D_LOG instead calls the IgnoreLog, hence discarding the print.
#define RAYX_D_LOG RAYX::IgnoreLog()
#define RAYX_D_WARN RAYX::IgnoreLog()
#define RAYX_D_ERR RAYX::IgnoreLog()
#define RAYX_D_VERB RAYX::IgnoreLog()
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

inline std::vector<double> formatAsVec(double arg) { return {arg}; }
inline std::vector<double> formatAsVec(EventType arg) { return {static_cast<double>(arg)}; }

inline std::vector<double> formatAsVec(complex::Complex comp) { return {comp.real(), comp.imag()}; }

inline std::vector<double> formatAsVec(const Ray arg) {
    std::vector<double> out;
    auto insert = [&out](const std::vector<double>& v) { out.insert(out.end(), v.begin(), v.end()); };

#define X(type, name, flag, map) insert(formatAsVec(arg.map));

        RAYX_X_MACRO_RAY_ATTR_MAPPED
#undef X

    return out;
}

template <int N, int M, typename T>
inline std::vector<double> formatAsVec(const glm::mat<N, M, T> arg) {
    std::vector<double> out;
    for (size_t i = 0; i < N * M; i++) {
        auto data = formatAsVec(arg[i / N][i % N]);
        out.insert(out.end(), data.begin(), data.end());
    }
    return out;
}

template <int N, typename T>
inline std::vector<double> formatAsVec(const glm::vec<N, T> arg) {
    std::vector<double> out;
    for (size_t i = 0; i < N; i++) {
        auto data = formatAsVec(arg[i]);
        out.insert(out.end(), data.begin(), data.end());
    }
    return out;
}

template <size_t N, typename T>
inline std::vector<double> formatAsVec(const std::array<T, N> arg) {
    std::vector<double> out;
    for (size_t i = 0; i < N; i++) {
        auto data = formatAsVec(arg[i]);
        out.insert(out.end(), data.begin(), data.end());
    }
    return out;
}

template <typename T>
inline std::vector<double> formatAsVec(const std::vector<T> arg) {
    std::vector<double> out;
    for (size_t i = 0; i < arg.size(); i++) {
        auto data = formatAsVec(arg[i]);
        out.insert(out.end(), data.begin(), data.end());
    }
    return out;
}

template <>
inline std::vector<double> formatAsVec<double>(const std::vector<double> arg) {
    return arg;
}

inline std::vector<double> formatAsVec(const RaySoA& rays) {
    std::vector<double> out;
    auto insert = [&out](const std::vector<double>& v) { out.insert(out.end(), v.begin(), v.end()); };

#define X(type, name, flag, map) insert(formatAsVec(rays.name));

    RAYX_X_MACRO_RAY_ATTR
#undef X

    return out;
}

void dbg(const std::string& filename, int line, std::string name, std::vector<double> v);

#define RAYX_DBG(C) RAYX::dbg(__FILE__, __LINE__, #C, RAYX::formatAsVec(C))

}  // namespace RAYX
