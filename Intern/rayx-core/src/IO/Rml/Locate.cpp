#include "Locate.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <cassert>
#else
#include <limits.h>
#include <unistd.h>
#endif

#include <Debug/Debug.h>

namespace rayx {

// Adds a new lookup path where the handler will search for resources
void ResourceHandler::addLookUpPath(const std::filesystem::path& path) {
    std::unique_lock lock(m_lookUpPathsMutex);

    // Check if the path is already in the lookUpPaths
    auto it = std::find(lookUpPaths.begin(), lookUpPaths.end(), path);
    if (it == lookUpPaths.end()) {
        // Insert at the beginning to prioritize newly added paths
        lookUpPaths.insert(lookUpPaths.begin(), path);
        RAYX_VERB << "adding lookup path for resources " << path;
    }
}

// Check if a file exists
bool ResourceHandler::fileExists(const std::string& path) { return std::filesystem::exists(path); }
bool ResourceHandler::fileExists(const std::filesystem::path& path) { return std::filesystem::exists(path); }

std::filesystem::path ResourceHandler::getExecutablePath() {
#if defined(_WIN32)
    // Start with a reasonable buffer size
    std::vector<char> buffer(MAX_PATH);
    while (true) {
        // Try to get the module filename
        DWORD size = GetModuleFileNameA(NULL, buffer.data(), buffer.size());
        if (size == 0) { return std::filesystem::path(); }
        // If the buffer was large enough, we're done
        if (size < buffer.size()) { break; }
        // Otherwise, increase buffer size and try again
        buffer.resize(buffer.size() * 2);
    }
#elif defined(__linux__)
    // Start with a reasonable buffer size, then grow dynamically if needed
    std::vector<char> buffer(PATH_MAX);
    ssize_t count;
    while (true) {
        count = readlink("/proc/self/exe", buffer.data(), buffer.size());
        if (count == -1) { return std::filesystem::path(); }
        if (count < static_cast<ssize_t>(buffer.size())) {
            buffer[count] = '\0';  // Null-terminate
            break;
        }
        // If the buffer was too small, increase size and retry
        buffer.resize(buffer.size() * 2);
    }

#else
    static_assert(false, "macOS support is not implemented yet");
#endif
    return std::filesystem::path(buffer.data());
}

// General method to get the full path based on the base directory (e.g., data or font directory)
std::filesystem::path ResourceHandler::getFullPath(const std::filesystem::path& baseDir, const std::filesystem::path& relativePath) {
    RAYX_VERB << "locating file: " << relativePath;
    auto found = [](std::filesystem::path path) {
        RAYX_VERB << "\t\tfound!";
        return path;
    };

    {
        std::shared_lock lock(m_lookUpPathsMutex); // lock lookUpPaths
        // First, check in user-defined lookup paths
        for (const auto& lookupPath : lookUpPaths) {
            std::filesystem::path path = lookupPath / baseDir / relativePath;
            RAYX_VERB << "\tlooking at " << path;
            if (fileExists(path)) { return found(path); }
        }
    }

#if defined(__linux__)
    // Check next to the executable (built from source)
    std::filesystem::path execDir = getExecutablePath().parent_path();
    std::filesystem::path path    = execDir / relativePath;
    RAYX_VERB << "\tlooking at " << path;
    if (fileExists(path)) return found(path);

    // Check in /usr (package install)
    path = std::filesystem::path("/usr") / baseDir / relativePath;
    RAYX_VERB << "\tlooking at " << path;
    if (fileExists(path)) return found(path);

    // Check in /usr/local (make install)
    path = std::filesystem::path("/usr/local") / baseDir / relativePath;
    RAYX_VERB << "\tlooking at " << path;
    if (fileExists(path)) return found(path);

#elif defined(_WIN32)
    // On Windows, only look next to the executable
    std::filesystem::path execDir = getExecutablePath().parent_path();
    std::filesystem::path path    = execDir / relativePath;
    RAYX_VERB << "\tlooking at " << path;
    if (fileExists(path)) return found(path);

#elif defined(__APPLE__)
    static_assert(false, "macOS support is not implemented yet");

#endif
    // Not found -> empty path
    RAYX_VERB << "\tfailed to locate file: " << relativePath;
    return std::filesystem::path();
}

// Retrieve the full path of a resource based on the platform
std::filesystem::path ResourceHandler::getResourcePath(const std::filesystem::path& relativePath) { return getFullPath(RAYX_DATA_DIR, relativePath); }

// Retrieve the full path of a font based on the platform
std::filesystem::path ResourceHandler::getFontPath(const std::filesystem::path& relativePath) { return getFullPath(RAYX_FONTS_DIR, relativePath); }

}  // namespace rayx
