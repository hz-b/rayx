#include "CsvWriter.h"

#include <bitset>
#include <set>
#include <sstream>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"

namespace RAYX {

RaySoA readCsvRaySoA(const std::filesystem::path& filename) {
    auto file = std::ifstream(filename);

    std::string line;

    // skip header line
    std::getline(file, line);

    RaySoA rays;
    rays.attr = RayAttrFlag::All;
    while (std::getline(file, line)) {
        std::stringstream ss(line);

        auto put = [&ss]<typename type>(type& var) {
            // get next token
            std::string cell_str;
            std::getline(ss, cell_str, ',');

            // create strinstream for the cell only
            std::stringstream ss_cell(cell_str);

            if constexpr (std::is_same_v<type, EventType>) {
                // trim leading and trailing whitespaces
                std::string value_str;
                ss_cell >> value_str;
                var = stringToEventType(value_str);
            } else if constexpr (std::is_floating_point_v<type>) {
                ss_cell >> var;
            } else if constexpr (std::is_integral_v<type>) {
                int64_t i;  // when var is of type int8_t, chars are treated as ascii, thus a temporary int is required
                ss_cell >> i;
                var = i;  // set var = i without explicit cast, to make the compiler detect potential narrowing conversions
            } else {
                RAYX_EXIT << "error: implemeneted!";
            }
        };

#define X(type, name, flag, map) rays.name.emplace_back();

        RAYX_X_MACRO_RAY_ATTR
#undef X

#define X(type, name, flag, map) put(rays.name.back());

        RAYX_X_MACRO_RAY_ATTR_BUT_ELECTRIC_FIELD
#undef X

        auto put_efield = [&](complex::Complex& efield) {
            double val;
            put(val);
            efield.real(val);
            put(val);
            efield.imag(val);
        };

        put_efield(rays.electric_field_x.back());
        put_efield(rays.electric_field_y.back());
        put_efield(rays.electric_field_z.back());

        ++rays.num_events;
    }

    rays.num_paths = std::set(rays.path_id.begin(), rays.path_id.end()).size();
    return rays;
}

BundleHistory readCsvBundleHistory(const std::filesystem::path& filename) {
    const auto rays = readCsvRaySoA(filename);
    return raySoAToBundleHistory(rays);
}

void writeCsvRaySoA(const std::filesystem::path& filename, const RaySoA& rays, const RayAttrFlag attr) {
    constexpr int PRECISION = 18;
    constexpr int CELL_WIDTH = 24;  // make space for extra characters e.g. minus sign or extra characters of scientific notation. also the names of
                                    // the attribute should fit

    auto file = std::ofstream(filename, std::ios::trunc);

    std::stringstream ss;
    ss << std::setprecision(PRECISION);
    ss << std::boolalpha;

#define X(type, name, flag, map) \
    if ((attr & RayAttrFlag::flag) != RayAttrFlag::None) ss << std::setw(CELL_WIDTH) << #flag << ",";

    RAYX_X_MACRO_RAY_ATTR_BUT_ELECTRIC_FIELD
#undef X

#define X(type, name, flag, map)                               \
    if ((attr & RayAttrFlag::flag) != RayAttrFlag::None) {     \
        ss << std::setw(CELL_WIDTH) << #flag " (real)" << ","; \
        ss << std::setw(CELL_WIDTH) << #flag " (imag)" << ","; \
    }

    RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD
#undef X

    auto s = ss.str();
    ss.str(std::string());

    if (!s.empty()) s.pop_back();

    file << s << std::endl;

    auto put = [&ss]<typename type>(const type value) {
        if constexpr (std::is_same_v<type, int8_t>)
            ss << std::setw(CELL_WIDTH) << static_cast<int>(value) << ",";
        else
            ss << std::setw(CELL_WIDTH) << value << ",";
    };

    for (int i = 0; i < rays.num_events; ++i) {
#define X(type, name, flag, map) \
    if ((attr & RayAttrFlag::flag) != RayAttrFlag::None) put(rays.name[i]);

        RAYX_X_MACRO_RAY_ATTR_BUT_ELECTRIC_FIELD
#undef X

#define X(type, name, flag, map)                           \
    if ((attr & RayAttrFlag::flag) != RayAttrFlag::None) { \
        put(rays.name[i].real());                          \
        put(rays.name[i].imag());                          \
    };

        RAYX_X_MACRO_RAY_ATTR_ELECTRIC_FIELD
#undef X

        s = ss.str();
        ss.str(std::string());

        if (!s.empty()) s.pop_back();

        file << s << std::endl;
    }
}

void writeCsvBundleHistory(const std::filesystem::path& filename, const BundleHistory& bundle, const RayAttrFlag attr) {
    const auto rays = bundleHistoryToRaySoA(bundle);
    writeCsvRaySoA(filename, rays, attr);
}

}  // namespace RAYX
