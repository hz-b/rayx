#include "CanonicalizePath.h"

#include <fstream>

#include "Debug/Debug.h"

/// this function assumes that `base` is already an absolute path
std::filesystem::path canonicalize(const std::filesystem::path& relPath, const std::filesystem::path& base) {
    if (!base.is_absolute()) {
        RAYX_ERR << "canonicalize called with non-absolute base path: \"" << base << "\"";
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

std::filesystem::path canonicalizeRepositoryPath(const std::filesystem::path& relPath) { return canonicalize(relPath, PROJECT_DIR); }

std::filesystem::path canonicalizeUserPath(const std::filesystem::path& relPath) { return canonicalize(relPath, std::filesystem::current_path()); }

// Read file into array of bytes, and cast to uint32_t*, then return.
// The data has been padded, so that it fits into an array uint32_t.
uint32_t* readFile(uint32_t& length, const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == nullptr) {
        printf("Could not find or open file: %s\n", filename);
    }

    // get file size.
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    if (filesize == -1) {
        RAYX_D_ERR << "Could not get file size.";
        return nullptr;
    }
    fseek(fp, 0, SEEK_SET);

    uint32_t filesizepadded = uint32_t(ceil(filesize / 4.0)) * 4;

    // read file contents.
    char* str = new char[filesizepadded];
    uint32_t readCount = fread(str, sizeof(char), filesize, fp);
    if (readCount != (uint32_t)filesize) {
        RAYX_D_WARN << readCount << " != " << filesize << "...";
        RAYX_D_ERR << "Errors while reading file: " << filename;
    }
    fclose(fp);

    // data padding.
    for (uint32_t i = filesize; i < filesizepadded; i++) {
        str[i] = 0;
    }

    length = filesizepadded;
    return (uint32_t*)str;
}

std::vector<uint8_t> readFile(const std::string& filename, const uint32_t count) {
    std::vector<uint8_t> data;

    std::ifstream file;

    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        RAYX_ERR << "Failed to open file: " << filename;
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
