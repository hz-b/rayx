#ifndef NO_H5

#include "H5Writer.h"

#include <bitset>
#include <highfive/highfive.hpp>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"

// declare user types for HighFive
namespace {
inline HighFive::DataType highfive_create_type_EventType() {
    return HighFive::EnumType<RAYX::EventType>({
        {"HitElement", RAYX::EventType::HitElement},
        {"TooManyEvents", RAYX::EventType::TooManyEvents},
        {"Absorbed", RAYX::EventType::Absorbed},
        {"Uninitialized", RAYX::EventType::Uninitialized},
        {"BeyondHorizon", RAYX::EventType::BeyondHorizon},
        {"FatalError", RAYX::EventType::FatalError},
        {"Emitted", RAYX::EventType::Emitted},
    });
}
}  // unnamed namespace
HIGHFIVE_REGISTER_TYPE(RAYX::EventType, highfive_create_type_EventType);

namespace RAYX {

/// get number of events, which is the number of entries in an active attribute
/// also check wether every active attribute has the same number of entries
int getNumEvents(const RaySoA& rays) {
    auto size = 0;
    auto resize = [&size](const auto& v) {
        const auto v_size = static_cast<int>(v.size());
        _assert(v_size == 0 || v_size == size || size == 0,
                "error: file corrupted: at least two non-empty Ray attributes have different number of elements!");
        size = std::max(size, v_size);
    };

#define RAYX_X(type, name, flag, map) resize(rays.name);

    RAYX_X_MACRO_RAY_ATTR_PATH_ID
    RAYX_X_MACRO_RAY_ATTR
#undef RAYX_X

    return size;
}

RaySoA readH5RaySoA(const std::filesystem::path& filepath, const RayAttrFlagType attr) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "reading rays from '" << filepath << "' with attribute flags: " << std::bitset<EndOfRayAttrBits>(attr);

    RaySoA rays;

    try {
        auto file = HighFive::File(filepath.string(), HighFive::File::ReadOnly);

        auto loadData = [&file](const auto& address, auto& dst) {
            file.getDataSet(address).read(dst);
            _assert(0 < dst.size(),
                    "attempting to load ray data (%s), but it is empty. Are you sure the file contains the requested data? Possible error is "
                    "insuficcient ray attribute flags when the data was written.",
                    address);
        };

#define RAYX_X(type, name, flag, map)                                                      \
    RAYX_VERB << "reading ray attribute: " #name " (" << rays.name.size() << " elements)"; \
    if (attr & flag) loadData("rayx/events/" #name, rays.name);

        RAYX_X_MACRO_RAY_ATTR_PATH_ID
        RAYX_X_MACRO_RAY_ATTR
#undef RAYX_X

        rays.num_paths = file.getDataSet("rayx/num_paths").read<int32_t>();
        rays.num_events = getNumEvents(rays);
    } catch (const std::exception& e) {
        RAYX_EXIT << "exception caught while attempting to read h5 file: " << e.what();
    }

    return rays;
}

BundleHistory readH5BundleHistory(const std::filesystem::path& filepath) {
    const auto rays = readH5RaySoA(filepath);
    return raySoAToBundleHistory(rays);
}

void writeH5RaySoA(const std::filesystem::path& filepath, const RaySoA& rays, const RayAttrFlagType attr) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "writing rays to '" << filepath << "' with attribute flags: " << std::bitset<EndOfRayAttrBits>(attr);

    try {
        auto file = HighFive::File(filepath.string(), HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);

#define RAYX_X(type, name, flag, map)                                                      \
    RAYX_VERB << "writing ray attribute: " #name " (" << rays.name.size() << " elements)"; \
    if (attr & flag) file.createDataSet("rayx/events/" #name, rays.name);

        RAYX_X_MACRO_RAY_ATTR_PATH_ID
        RAYX_X_MACRO_RAY_ATTR
#undef RAYX_X

        file.createDataSet("rayx/num_paths", rays.num_paths);
    } catch (const std::exception& e) {
        RAYX_EXIT << "exception caught while attempting to write h5 file: " << e.what();
    }
}

void writeH5BundleHistory(const std::filesystem::path& filepath, const BundleHistory& bundle, const RayAttrFlagType attr) {
    const auto rays = bundleHistoryToRaySoA(bundle);
    writeH5RaySoA(filepath, rays, attr);
}

}  // namespace RAYX

#endif
