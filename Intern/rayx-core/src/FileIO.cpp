#include <fstream>

#include "CanonicalizePath.h"
#include "Debug/Debug.h"

namespace RAYX {

std::optional<std::vector<uint8_t>> readFile(const std::string& filename, const uint32_t count) {
    std::vector<uint8_t> data;

    std::ifstream file;

    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        return {};
    }

    uint64_t read_count = count;
    if (count == 0) {
        file.seekg(0, std::ios::end);
        read_count = static_cast<uint64_t>(file.tellg());
        file.seekg(0, std::ios::beg);
    }

    data.resize(static_cast<size_t>(read_count));
    file.read(reinterpret_cast<char*>(data.data()), read_count);
    file.close();

    return data;
}

// Read file into array of bytes, and cast to uint32_t*, then return.
// The data has been padded, so that it fits into an array uint32_t.
std::optional<std::vector<uint32_t>> readFileAlign32(const std::string& filename, const uint32_t count) {
    std::vector<uint32_t> data;

    std::ifstream file;

    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        return {};
    }

    uint64_t read_count = count;
    if (count == 0) {
        file.seekg(0, std::ios::end);
        read_count = static_cast<uint64_t>(file.tellg());
        file.seekg(0, std::ios::beg);
    }

    // Check, if it's even uint32_t alignable!
    if (read_count % sizeof(uint32_t) != 0) {
        return {};
    }

    data.resize(static_cast<size_t>(read_count) / sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(data.data()), read_count);
    file.close();

    return data;
}

void writeFile(const std::vector<uint8_t>& data, const std::string& filename, const uint32_t count) {
    std::ofstream file;

    file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    if (!file.is_open()) {
        RAYX_ERR << "Failed to open file: " << filename;
    }

    uint64_t write_count = count;
    if (count == 0) {
        write_count = data.size();
    }

    file.write(reinterpret_cast<const char*>(data.data()), write_count);
    file.close();
}

}  // namespace RAYX
