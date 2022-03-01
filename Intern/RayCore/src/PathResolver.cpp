#include "PathResolver.h"

#include <Debug.h>

#include <filesystem>
#include <optional>

static std::optional<std::filesystem::path> ROOT;

void initPathResolver(char* executablePath) {
    std::filesystem::path p = std::filesystem::canonical(
        executablePath);  // rayreworked/build/bin/TerminalApp
    p.remove_filename();  // rayreworked/build/bin/
    p.append("..");       // rayreworked/build
    p.append("..");       // rayreworked
    ROOT = p;
}

std::string resolvePath(std::string path) {
    if (!ROOT) {
        RAYX_ERR
            << "can not resolve path without prior call to initPathResolver";
    }

    std::filesystem::path p = *ROOT;
    p.append(path);
    return p;
}
