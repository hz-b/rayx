#pragma once

#include <string>

// the path resolver has to be initialized with the path of the executable: argv[0].
void initPathResolver(char* executablePath);

/** @param path: path relative to the root of the repository.
 * Converts this path to be relative to the current working directory, so that
 * it may be used by eg. open()
 */
std::string resolvePath(std::string path);