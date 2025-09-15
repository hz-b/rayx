#pragma once

#include <optional>
#include <string>
#include <vector>

struct CliArgs {
    bool plot        = false;                 // -p --plot
    bool csv         = false;                 // -c --csv
    bool cpu         = false;                 // -x --cpu
    bool gpu         = false;                 // -X --gpu
    bool listDevices = false;                 // -l --list-devices
    bool benchmark   = false;                 // -B --benchmark
    bool version     = false;                 // -v --version
    bool sequential  = false;                 // -S --sequential
    bool verbose     = false;                 // -V --verbose
    bool defaultSeed = false;                 // -f, --default-seed
    std::optional<int> numberOfRays;          // -n --number-of-rays
    std::optional<int> maxEvents;             // -m --maxevents
    std::optional<std::string> dump;          // -D --dump
    std::vector<std::string> inputPaths;      // -i --input
    std::optional<std::string> outputPath;    // -o --output
    std::optional<int> seed;                  // -s, --seed
    std::optional<int> batchSize;             // -b --batch-size
    std::optional<int> deviceId;              // -d --device
    std::vector<int> objectRecordIndices;     // -R --record-indices
    std::vector<std::string> attrRecordMask;  // -A --attributes
};

CliArgs parseCliArgs(const int argc, char const* const* const argv);
