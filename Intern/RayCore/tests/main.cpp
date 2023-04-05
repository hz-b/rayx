#include <filesystem>

#include "CanonicalizePath.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "setupTests.h"

int GLOBAL_ARGC = 0;
char** GLOBAL_ARGV = nullptr;

int main(int argc, char** argv) {
    std::filesystem::path outputDir = canonicalizeRepositoryPath("Intern/RayCore/tests/output");
    if (!std::filesystem::is_directory(outputDir) || !std::filesystem::exists(outputDir)) {
        std::filesystem::create_directory(outputDir);
    }

    testing::InitGoogleTest(&argc, argv);

    GLOBAL_ARGC = argc;
    GLOBAL_ARGV = argv;

    // stop execution of tests as soon as the first fails
    testing::FLAGS_gtest_break_on_failure = "true";

    return RUN_ALL_TESTS();
}
