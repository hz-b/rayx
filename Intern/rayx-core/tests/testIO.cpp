#include "setupTests.h"

// Prepare test data
std::vector<uint8_t> testData = {'1', '2', '3', '4', '5'};  // ASCII values
std::string readFilename      = canonicalizeRepositoryPath("Intern/rayx-core/tests/input/read_test.txt").string();
std::string writeFilename     = canonicalizeRepositoryPath("Intern/rayx-core/tests/input/write_test.txt").string();

// TEST_F(TestSuite, TestReadFileIntoArray) {
//     uint32_t length;
//     auto *data = readFile(length, readFilename.c_str());

//     // Check if the data was read correctly
//     for (uint32_t i = 0; i < length; i++) {
//         ASSERT_EQ(static_cast<uint32_t>(testData[i]), data[i]);
//     }

//     delete[] data;
// }

TEST_F(TestSuite, TestReadFileIntoVector) {
    auto data = readFile(readFilename, testData.size()).value();

    // Check if the data was read correctly
    for (uint32_t i = 0; i < testData.size(); i++) { ASSERT_EQ(testData[i], data[i]); }
}

TEST_F(TestSuite, TestWriteFileFromVector) {
    writeFile(testData, writeFilename, testData.size());

    // Now read the file and check if the data was written correctly
    std::ifstream readTestFile(writeFilename, std::ios::binary);
    std::vector<uint8_t> readData((std::istreambuf_iterator<char>(readTestFile)), std::istreambuf_iterator<char>());

    for (uint32_t i = 0; i < testData.size(); i++) { ASSERT_EQ(testData[i], readData[i]); }
}
