#pragma once

#include <filesystem>
#include <vector>

#include "Core.h"
#include "FileIO.h"

namespace RAYX {

/// `relPath` is a path relative to the root of the RAY-X git repository (i.e.
/// where .git lies). canonicalizeRepositoryPath(relPath) yields an absolute
/// path representing the same path. Examples:
/// - canonicalizeRepositoryPath(/home/username/foo) = /home/username/foo #
/// absolute paths are unchanged!
/// - canonicalizeRepositoryPath(build/bin/python) =
/// /home/username/path/to/RAY-X/build/bin/python
/// - canonicalizeRepositoryPath(Tests/input) =
/// /home/username/path/to/RAY-X/Tests/input
std::filesystem::path RAYX_API canonicalizeRepositoryPath(const std::filesystem::path& relPath);

/// `relPath` is a path relative to the working directory of the user executing
/// RAY-X at the point of execution. canonicalizeUserPath(relPath) yields an
/// absolute path representing the same path. Examples:
/// - canonicalizeUserPath(/home/username/file) = /home/username/file # absolute
/// paths are unchanged!
/// - if you are at /home/username/folder, then canonicalizeUserPath(file) =
/// /home/username/folder/file
///
/// This function is used to find the correct path when the user executes
/// `./TerminalApp -i <relPath>`
std::filesystem::path RAYX_API canonicalizeUserPath(const std::filesystem::path& relPath);

}  // namespace RAYX
