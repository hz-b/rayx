#include "Locate.h"
#include <iostream>
#include <filesystem>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <cassert>
#else
#include <unistd.h>
#include <limits.h>
#endif

namespace RAYX {

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
        if (size == 0) {
            return std::filesystem::path();
        }
        // If the buffer was large enough, we're done
        if (size < buffer.size()) {
            break;
        }
        // Otherwise, increase buffer size and try again
        buffer.resize(buffer.size() * 2);
    }
#elif defined(__linux__)
    // Start with a reasonable buffer size, then grow dynamically if needed
    std::vector<char> buffer(PATH_MAX);
    ssize_t count;
    while (true) {
        count = readlink("/proc/self/exe", buffer.data(), buffer.size());
        if (count == -1) {
            return std::filesystem::path();
        }
        if (count < static_cast<ssize_t>(buffer.size())) {
            buffer[count] = '\0';  // Null-terminate
            break;
        }
        // If the buffer was too small, increase size and retry
        buffer.resize(buffer.size() * 2);
    }

#else
    throw std::runtime_error("getExecutablePath is not implemented for this platform");
#endif
    return std::filesystem::path(buffer.data());
}

// General method to get the full path based on the base directory (e.g., data or font directory)
std::filesystem::path ResourceHandler::getFullPath(const std::string& baseDir, const std::string& relativePath) {
#if defined(__linux__)
    // First, check in /usr (package install)
    std::string path = std::string("/usr/") + baseDir + "/" + relativePath;
    if (fileExists(path)) return path;

    // Next, check next to the executable (built from source)
    std::string execDir = getExecutablePath().string();
    execDir = execDir.substr(0, execDir.find_last_of("/\\"));
    path = execDir + "/" + relativePath;
    if (fileExists(path)) return path;

    // Lastly, check in /usr/local (make install)
    path = std::string("/usr/local/") + baseDir + "/" + relativePath;
    if (fileExists(path)) return path;
#elif defined(_WIN32)
    // On Windows, only look next to the executable
    std::string execDir = getExecutablePath().string();
    execDir = execDir.substr(0, execDir.find_last_of("/\\"));
    std::filesystem::path path = std::filesystem::path(execDir + "\\" + relativePath);
    if (fileExists(path)) return path;
#elif defined(__APPLE__)
    static_assert(false, "macOS support is not implemented yet");

#endif
    // Not found -> empty path
    return std::filesystem::path();
}

// Retrieve the full path of a resource based on the platform
std::filesystem::path ResourceHandler::getResourcePath(const std::string& relativePath) { return getFullPath(RAYX_DATA_DIR, relativePath); }

// Retrieve the full path of a font based on the platform
std::filesystem::path ResourceHandler::getFontPath(const std::string& relativePath) { return getFullPath(RAYX_FONTS_DIR, relativePath); }

}  // namespace RAYX