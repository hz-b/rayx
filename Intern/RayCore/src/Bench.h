#pragma once

#include <chrono>
#include <iostream>
#include <map>

#include "Core.h"

#define BENCH auto __benchvar__ = Bench(__func__)
#define BENCH_NAMED(s) auto __benchvar_named__ = Bench(s)

using namespace std::chrono;

extern RAYX_API bool BENCH_FLAG;
extern RAYX_API std::map<std::string, double> bench_table;

class RAYX_API Bench {
  public:
    Bench(std::string s) {
        if (BENCH_FLAG) {
            string = s;
            start = high_resolution_clock::now();
        }
    }

    ~Bench() {
        if (BENCH_FLAG) {
            auto end = high_resolution_clock::now();
            double delta = (double)duration_cast<milliseconds>(end - start).count();
            double delta_s = delta / 1000.0;
            std::cout << "BENCH: \"" << string << "\" took " << delta_s << "s\n";
            bench_table[string] = delta_s;
        }
    }

  private:
    time_point<high_resolution_clock> start;
    std::string string;
};
