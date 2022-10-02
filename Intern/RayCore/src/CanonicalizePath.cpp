#include "CanonicalizePath.h"

#include <Debug.h>

/// this function assumes that `base` is already an absolute path
std::filesystem::path canonicalize(const std::filesystem::path& relPath,
                                   const std::filesystem::path& base) {
    if (!base.is_absolute()) {
        RAYX_ERR << "canonicalize called with non-absolute base path: \""
                 << base << "\"";
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

std::filesystem::path RAYX_API
canonicalizeRepositoryPath(const std::filesystem::path& relPath) {
    return canonicalize(relPath, PROJECT_DIR);
}

std::filesystem::path RAYX_API
canonicalizeUserPath(const std::filesystem::path& relPath) {
    return canonicalize(relPath, std::filesystem::current_path());
}
