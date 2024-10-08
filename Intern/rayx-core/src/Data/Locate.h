#pragma once

#include <filesystem>
#include <string>
#include <set>

#include "Core.h"

namespace RAYX {

class RAYX_API ResourceHandler {
  public:
    static ResourceHandler& getInstance() {
        thread_local ResourceHandler instance;
        return instance;
    }

    // Platform-specific implementations for resource path search
    std::filesystem::path getExecutablePath();

    // Retrieves a resource file's full path based on the relative path
    std::filesystem::path getResourcePath(const std::string& relativePath);

    // Retrieves a font file's full path based on the relative path
    std::filesystem::path getFontPath(const std::string& relativePath);

  private:
    ResourceHandler() = default;
    bool fileExists(const std::string& path);
    bool fileExists(const std::filesystem::path& path);
    std::filesystem::path getFullPath(const std::string& baseDir, const std::string& relativePath);

    std::set<std::filesystem::path> lookUpPaths = {
#if defined(__linux__)
        std::filesystem::path("/usr"),        //
        std::filesystem::path("/usr/local"),  //
#endif
    };
};

}  // namespace RAYX