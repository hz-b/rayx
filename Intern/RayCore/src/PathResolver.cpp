#include "PathResolver.h"

#include <Debug.h>

static std::optional<std::filesystem::path> ROOT;

void initPathResolver() {
    std::filesystem::path p = std::filesystem::canonical(
        PROJECT_DIR);
    ROOT = p;
}

std::string resolvePath(const std::string& path) {
    if (!ROOT) {
        RAYX_ERR
            << "can not resolve path without prior call to initPathResolver";
    }

    std::filesystem::path p = *ROOT;
    p.append(path);
    return p.string();
}

std::string getFilename(char* path) {
    return std::filesystem::path(std::string(path)).filename().string();
}

std::string getFilename(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}