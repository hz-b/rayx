#include <filesystem>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

int main(int argc, char** argv) {
    {
        const char* outputDir = "../../Tests/output/";
        if (!std::filesystem::is_directory(outputDir) ||
            !std::filesystem::exists(outputDir)) {
            std::filesystem::create_directory(outputDir);
        }
    }

    testing::InitGoogleTest(&argc, argv);
    // stop execution of tests as soon as the first fails
    testing::FLAGS_gtest_fail_fast = "fast";

    return RUN_ALL_TESTS();
}