//
// Basic instrumentation profiler by Cherno

#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "Core.h"

namespace rayx {

extern bool RAYX_API BENCH_FLAG;

class RAYX_API InstrumentationTimer {
  public:
    InstrumentationTimer(const char* name, bool canPrint) : m_Name(name), m_isStopped(false), m_canPrint(canPrint) {
        if (BENCH_FLAG) m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~InstrumentationTimer() {
        if (!m_isStopped) Stop();
    }

    void Stop() {
        if (BENCH_FLAG) {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end   = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            if (m_canPrint) {
                long long duration = end - start;
                double seconds     = duration * 0.000001;
                std::cout << "BENCH: " << m_Name << ":" << std::endl << seconds << "s" << std::endl;
            }

            m_isStopped = true;
        }
    }

  private:
    const char* m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    bool m_isStopped;
    bool m_canPrint;
};

}  // namespace rayx

// Define profiling macros
#define RAYX_PROFILE_SCOPE(name) ::rayx::InstrumentationTimer timer##__LINE__(name, false)
// Allows for printing of benchmarking results if BENCH_FLAG is set to true
#define RAYX_PROFILE_SCOPE_STDOUT(name) ::rayx::InstrumentationTimer timer##__LINE__(name, true)
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#define RAYX_PROFILE_FUNCTION() RAYX_PROFILE_SCOPE(__PRETTY_FUNCTION__)
// Allows for printing of benchmarking results if BENCH_FLAG is set to true
#define RAYX_PROFILE_FUNCTION_STDOUT() RAYX_PROFILE_SCOPE_STDOUT(__func__)
