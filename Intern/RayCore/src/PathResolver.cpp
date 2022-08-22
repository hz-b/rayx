#include "PathResolver.h"

#include <Debug.h>

static std::optional<std::filesystem::path> ROOT;

void RAYX_API initPathResolver(const char* executablePath) {
    std::filesystem::path p = std::filesystem::canonical(
        executablePath);  // ray-x/build/bin/TerminalApp
    p = p.parent_path();  // ray-x/build/bin/
    p = p.parent_path();  // ray-x/build
    p = p.parent_path();  // ray-x
    ROOT = p;
}

std::string RAYX_API resolvePath(const std::string& path) {
    if (!ROOT) {
        RAYX_ERR
            << "can not resolve path without prior call to initPathResolver";
    }

    std::filesystem::path p = *ROOT;
    p.append(path);
    return p.string();
}

std::string RAYX_API getFilename(char* path) {
    return std::filesystem::path(std::string(path)).filename().string();
}

std::string RAYX_API getFilename(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}