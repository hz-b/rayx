#include "CanonicalizePath.h"

#include <cstring>
#include <stdexcept>

#include "Debug/Debug.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#elif defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
#include <limits.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

namespace RAYX {

std::filesystem::path getExecutablePath() {
    char buffer[1024];
    std::size_t length = sizeof(buffer);
    memset(buffer, 0, length);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // Windows implementation
    DWORD ret = GetModuleFileNameA(NULL, buffer, length);
    if (ret == 0 || ret == length) {
        // Handle error
        throw std::runtime_error("Failed to get executable path.");
    }
#elif defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
    // Linux and Unix implementation
    ssize_t ret = readlink("/proc/self/exe", buffer, length - 1);
    if (ret == -1) {
        // Handle error
        throw std::runtime_error("Failed to get executable path.");
    }
    buffer[ret] = '\0';  // Ensure null-terminated string
#elif defined(__APPLE__)
    // macOS implementation
    if (_NSGetExecutablePath(buffer, &length) != 0) {
        // Handle error
        throw std::runtime_error("Buffer too small; should not happen.");
    }
#else
#error "Platform not supported."
#endif

    return std::filesystem::path(buffer).parent_path();
}

/// this function assumes that `base` is already an absolute path
std::filesystem::path canonicalize(const std::filesystem::path& relPath, const std::filesystem::path& base) {
    if (!base.is_absolute()) {
        RAYX_EXIT << "canonicalize called with non-absolute base path: \"" << base << "\"";
    }
    // absolute paths will stay unchanged
    if (relPath.is_absolute()) {
        return relPath;
    }
    // relative paths will be joined onto the base:
    // canonicalize("../foo/bar", "/home/username/RAY-X") =
    // "/home/username/RAY-X/../foo/bar" = "/home/username/foo/bar"
    return base / relPath;
}

std::filesystem::path canonicalizeRepositoryPath(const std::filesystem::path& relPath) { return canonicalize(relPath, RAYX_PROJECT_DIR); }

std::filesystem::path canonicalizeUserPath(const std::filesystem::path& relPath) { return canonicalize(relPath, std::filesystem::current_path()); }

}  // namespace RAYX
