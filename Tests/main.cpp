#include <filesystem>

#include "PathResolver.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

int main(int argc, char** argv) {
    initPathResolver(argv[0]);
    {
        std::filesystem::path outputDir = resolvePath("Tests/output");
        if (!std::filesystem::is_directory(outputDir) ||
            !std::filesystem::exists(outputDir)) {
            std::filesystem::create_directory(outputDir);
        }
    }

    testing::InitGoogleTest(&argc, argv);

    // stop execution of tests as soon as the first fails
    testing::FLAGS_gtest_break_on_failure = "true";

    return RUN_ALL_TESTS();
}
