#include "CanonicalizePath.h"

#include <cstring>
#include <stdexcept>

#include "IO/Debug.h"

namespace rayx {

/// this function assumes that `base` is already an absolute path
std::filesystem::path canonicalize(const std::filesystem::path& relPath, const std::filesystem::path& base) {
    if (!base.is_absolute()) { RAYX_EXIT << "canonicalize called with non-absolute base path: \"" << base << "\""; }
    // absolute paths will stay unchanged
    if (relPath.is_absolute()) { return relPath; }
    // relative paths will be joined onto the base:
    // canonicalize("../foo/bar", "/home/username/RAY-X") =
    // "/home/username/RAY-X/../foo/bar" = "/home/username/foo/bar"
    return base / relPath;
}

std::filesystem::path canonicalizeRepositoryPath(const std::filesystem::path& relPath) { return canonicalize(relPath, RAYX_PROJECT_DIR); }

std::filesystem::path canonicalizeUserPath(const std::filesystem::path& relPath) { return canonicalize(relPath, std::filesystem::current_path()); }

}  // namespace rayx
