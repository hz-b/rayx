#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "Core.h"

// the path resolver has to be initialized with the path of the executable:
// argv[0].
void RAYX_API initPathResolver();

/** @param path: path relative to the root of the repository.
 * Converts this path to be relative to the current working directory, so that
 * it may be used by eg. open()
 */
std::string RAYX_API resolvePath(const std::string& path);

/**
 * @brief Get the Filename inside a path (Tail)
 *
 * @param path ex: C:\test\hello.txt
 * @return std::string  ex: hello.txt
 */
std::string RAYX_API getFilename(char* path);
std::string RAYX_API getFilename(const std::string& path);
