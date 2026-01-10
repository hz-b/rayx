#include "CsvWriter.h"

#include <filesystem>
#include <fstream>
#include <queue>

#include "Misc/StringConversion.h"
#include "IO/Debug.h"

namespace rayx {

namespace fs = std::filesystem;

namespace {

constexpr int PADDING = 0;  // extra spaces on the left side in each cell for better readability

// see https://stackoverflow.com/questions/1701055/what-is-the-maximum-length-in-chars-needed-to-represent-any-double-value
// constexpr int MAX_CELL_SIZE_FLOAT  = 16 + PADDING;
constexpr int MAX_CELL_SIZE_DOUBLE = 24 + PADDING;
constexpr int MAX_CELL_SIZE_INT    = 11 + PADDING;
constexpr int MAX_CELL_SIZE_UINT64 = 20 + PADDING;
constexpr char DELIMITER           = ',';

std::string trimWhitespaces(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}

std::string formatAsString(const double v) {
    // std::format gives us the exact number of digits, that correctly represent the double.
    return std::format("{}", v);
}

std::string formatAsString(const int v) { return std::to_string(v); }

std::string formatAsString(const EventType v) { return EventTypeToString.at(v); }

std::string formatAsString(const RandCounter v) { return std::to_string(v); }

template <typename T>
int calcCellSize(const std::string header);

template <>
int calcCellSize<double>(const std::string header) {
    return std::max(MAX_CELL_SIZE_DOUBLE, static_cast<int>(header.size()) + PADDING);
}

template <>
int calcCellSize<int>(const std::string header) {
    return std::max(MAX_CELL_SIZE_INT, static_cast<int>(header.size()) + PADDING);
}

template <>
int calcCellSize<EventType>(const std::string header) {
    int maxSize = 0;
    for (const auto& [_, str] : EventTypeToString) maxSize = std::max(maxSize, static_cast<int>(str.size()));
    return std::max(maxSize, static_cast<int>(header.size()) + PADDING);
}

template <>
int calcCellSize<RandCounter>(const std::string header) {
    return std::max(MAX_CELL_SIZE_UINT64, static_cast<int>(header.size()) + PADDING);
}

std::vector<int> calcCellSizes(const RayAttrMask attr) {
    std::vector<int> cellSizes;

    auto addCellSize = [&]<typename T>(const std::string& name, const RayAttrMask flag) {
        if constexpr (std::is_same_v<T, complex::Complex>) {
            if (contains(attr, flag)) cellSizes.push_back(calcCellSize<typename T::value_type>(name));
        } else {
            if (contains(attr, flag)) cellSizes.push_back(calcCellSize<T>(name));
        }
    };

#define X(type, name, flag) addCellSize.operator()<type>(#name, RayAttrMask::flag);
    RAYX_X_MACRO_RAY_ATTR
#undef X

    return cellSizes;
}

std::string formatAsCell(std::string s, const int size) {
    if (size < static_cast<int>(s.size()))
        RAYX_EXIT << "cell: string \"" << s << "\" needs to be shortened! maximum size: " << size << ", actual size: " << s.size();
    s.insert(0, size - s.size(), ' ');
    return s;
}

template <typename T>
std::string formatAsCell(const T v, const int size) {
    return formatAsCell(formatAsString(v), size);
}

void writeCsvHeader(std::ostream& os, const RayAttrMask attr, const std::vector<int>& cellSizes) {
    const auto numAttr = countSetBits(attr);
    int attrCount      = 0;

    auto writeCell = [&]<typename T>(const std::string& name, const RayAttrMask flag) {
        if constexpr (std::is_same_v<T, complex::Complex>) {
            if (contains(attr, flag)) {
                const auto cellSize = cellSizes.at(attrCount);
                os << formatAsCell(name + " (real)", cellSize) << DELIMITER;
                os << formatAsCell(name + " (imag)", cellSize);
                if (++attrCount < numAttr) os << DELIMITER;
            }
        } else {
            if (contains(attr, flag)) {
                os << formatAsCell(name, cellSizes.at(attrCount));
                if (++attrCount < numAttr) os << DELIMITER;
            }
        }
    };

#define X(type, name, flag) writeCell.operator()<type>(#name, RayAttrMask::flag);
    RAYX_X_MACRO_RAY_ATTR
#undef X
}

void writeCsvBodyLine(std::ostream& os, const int i, const RayAttrMask attr, const Rays& rays, const std::vector<int>& cellSizes) {
    const auto numAttr = countSetBits(attr);
    auto attrCount     = 0;

    auto writeCell = [&]<typename T>(const std::vector<T>& src, const RayAttrMask flag) {
        if constexpr (std::is_same_v<T, complex::Complex>) {
            if (contains(attr, flag)) {
                os << formatAsCell(src[i].real(), cellSizes.at(attrCount)) << DELIMITER;
                os << formatAsCell(src[i].imag(), cellSizes.at(attrCount));
                if (++attrCount < numAttr) os << DELIMITER;
            }
        } else {
            if (contains(attr, flag)) {
                os << formatAsCell(src[i], cellSizes.at(attrCount));
                if (++attrCount < numAttr) os << DELIMITER;
            }
        }
    };

#define X(type, name, flag) writeCell(rays.name, RayAttrMask::flag);
    RAYX_X_MACRO_RAY_ATTR
#undef X
}

template <typename T>
T readCell(const std::string& cell);

template <>
double readCell<double>(const std::string& cell) {
    return std::stod(cell);
}

template <>
int readCell<int>(const std::string& cell) {
    return std::stoi(cell);
}

template <>
EventType readCell<EventType>(const std::string& cell) {
    return StringToEventType.at(trimWhitespaces(cell));
}

template <>
RandCounter readCell<RandCounter>(const std::string& cell) {
    // check if the usage of std::stoull is correct
    static_assert(std::is_same_v<RandCounter, uint64_t>);
    static_assert(sizeof(RandCounter) <= sizeof(decltype(std::stoull(cell))));
    return std::stoull(cell);
}

std::vector<RayAttrMask> readCsvHeader(const std::string& line) {
    std::queue<std::string> attrStrings;
    {
        auto ss = std::istringstream(line);
        std::string item;
        while (std::getline(ss, item, DELIMITER)) attrStrings.push(trimWhitespaces(item));
    }

    auto attrs = std::vector<RayAttrMask>();

    auto consumeItem = [&](const std::string& name, const RayAttrMask flag) {
        if (attrStrings.front() == name) {
            attrs.push_back(flag);
            attrStrings.pop();
            return true;
        } else if (attrStrings.front() == name + " (real)") {
            attrs.push_back(flag);
            attrStrings.pop();
            if (attrStrings.empty() || attrStrings.front() != name + " (imag)")
                RAYX_EXIT << "error: expected imaginary part of complex number after real part";
            attrStrings.pop();
            return true;
        }

        return false;
    };

    while (!attrStrings.empty()) {
#define X(type, name, flag) \
    if (consumeItem(#name, RayAttrMask::flag)) continue;
        RAYX_X_MACRO_RAY_ATTR
#undef X

        RAYX_EXIT << "error: unknown csv header: '" << attrStrings.front() << "'";
    }

    return attrs;
}

void readCsvBodyLine(Rays& rays, const std::vector<RayAttrMask>& attrs, const std::string& line) {
    auto ss = std::istringstream(line);
    std::string item;
    auto consumeCell = [&]<typename T>(std::vector<T>& dst) {
        if constexpr (std::is_same_v<T, complex::Complex>) {
            std::getline(ss, item, DELIMITER);
            const auto real = readCell<typename T::value_type>(item);
            std::getline(ss, item, DELIMITER);
            const auto imag = readCell<typename T::value_type>(item);
            dst.emplace_back(real, imag);
        } else {
            std::getline(ss, item, DELIMITER);
            dst.push_back(readCell<T>(item));
        }
    };

    for (const auto attr : attrs) {
        switch (attr) {
#define X(type, name, flag)     \
    case RayAttrMask::flag:     \
        consumeCell(rays.name); \
        break;
            RAYX_X_MACRO_RAY_ATTR
#undef X
            default:
                RAYX_EXIT << "error: unknown attribute: '" << to_string(attr) << "'";
        }
    }
}

}  // namespace

void writeCsv(const fs::path& filepath, const Rays& rays) {
    const auto attr      = rays.attrMask();
    const auto cellSizes = calcCellSizes(attr);

    auto file = std::ofstream(filepath);

    writeCsvHeader(file, attr, cellSizes);
    file << '\n';

    const auto size = rays.size();
    for (int i = 0; i < size; i++) {
        writeCsvBodyLine(file, i, attr, rays, cellSizes);
        file << '\n';
    }
}

Rays readCsv(const fs::path& filepath) {
    auto file = std::ifstream(filepath);
    std::string line;
    std::getline(file, line);
    const auto attrs = readCsvHeader(line);

    Rays rays;
    while (std::getline(file, line)) readCsvBodyLine(rays, attrs, line);

    if (!rays.isValid()) RAYX_EXIT << "error: one or more recorded attributes have different number of items";
    return rays;
}

}  // namespace rayx
