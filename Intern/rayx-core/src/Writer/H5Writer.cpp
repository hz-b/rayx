#ifndef NO_H5

#include "H5Writer.h"

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

inline HighFive::DataType highfive_create_type_Complex() {
    return HighFive::CompoundType({
        {"r", HighFive::AtomicType<double>(), 0},
        {"i", HighFive::AtomicType<double>(), sizeof(double)},
    });
}
}  // unnamed namespace
HIGHFIVE_REGISTER_TYPE(RAYX::EventType, highfive_create_type_EventType);
HIGHFIVE_REGISTER_TYPE(RAYX::complex::Complex, highfive_create_type_Complex);

namespace RAYX {

// TODO: this function should not require, that attr is known beforehand. Mabye we should use attr only to further exclude attributes? Or provide an
// extra attr that is repsonsible to check for existence?
Rays readH5Rays(const std::filesystem::path& filepath, const RayAttrMask attr) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "reading rays from " << filepath << " with attribute flags: " << to_string(attr);

    Rays rays;

    try {
        auto file = HighFive::File(filepath.string(), HighFive::File::ReadOnly);

        auto loadData = [&file](const auto& address, auto& dst) {
            file.getDataSet(address).read(dst);
            _assert(0 < dst.size(),
                    "attempting to load ray data (%s), but it is empty. Are you sure the file contains the requested data? Possible cause is "
                    "insuficcient ray attribute flags when the data was written.",
                    address);
        };

#define X(type, name, flag)                                                                \
    RAYX_VERB << "reading ray attribute: " #name " (" << rays.name.size() << " elements)"; \
    if (contains(attr, RayAttrMask::flag)) loadData("rayx/events/" #name, rays.name);

        RAYX_X_MACRO_RAY_ATTR
#undef X
    } catch (const std::exception& e) { RAYX_EXIT << "exception caught while attempting to read h5 file: " << e.what(); }

    return rays;
}

std::vector<std::string> readH5ObjectNames(const std::filesystem::path& filepath) {
    RAYX_VERB << "reading element names from " << filepath;

    auto object_names = std::vector<std::string>();

    try {
        auto file = HighFive::File(filepath.string(), HighFive::File::ReadOnly);

        file.getDataSet("/rayx/object_names").read(object_names);
    } catch (const std::exception& e) { RAYX_EXIT << "exception caught while attempting to read h5 file: " << e.what(); }

    return object_names;
}

void writeH5(const std::filesystem::path& filepath, const std::vector<std::string>& object_names, const Rays& rays, const RayAttrMask attr,
             const bool overwrite) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "write rays to " << filepath << " with attribute flags: " << to_string(attr);

    if (!contains(rays.attrMask(), attr))
        RAYX_EXIT << "Cannot write rays to output file '" << filepath
                  << "' because the rays do not contain all attributes specified in the attribute mask: " << to_string(attr)
                  << ". The rays contain the following attributes: " << to_string(rays.attrMask());

    try {
        const auto flags = HighFive::File::ReadWrite | HighFive::File::Create | (overwrite ? HighFive::File::Truncate : HighFive::File::Excl);
        auto file        = HighFive::File(filepath.string(), flags);

#define X(type, name, flag)                                                              \
    RAYX_VERB << "write ray attribute: " #name " (" << rays.name.size() << " elements)"; \
    if (contains(attr, RayAttrMask::flag)) file.createDataSet("rayx/events/" #name, rays.name);

        RAYX_X_MACRO_RAY_ATTR
#undef X

        // TODO: store RayAttrMask
        file.createDataSet("rayx/num_events", rays.size());
        file.createDataSet("rayx/object_names", object_names);
    } catch (const std::exception& e) { RAYX_EXIT << "exception caught while attempting to write h5 file: " << e.what(); }
}

void appendH5(const std::filesystem::path& filepath, const Rays& rays, const RayAttrMask attr) {
    // TODO: fix this function
    assert(false && "unimplemented");

    RAYX_PROFILE_FUNCTION_STDOUT();
    RAYX_VERB << "append rays to " << filepath << " with attribute flags: " << to_string(attr);

    if (!std::filesystem::is_regular_file(filepath))
        RAYX_EXIT << "Cannot append to output file '" << filepath << "' because it does not exist or is not a regular file.";

    try {
        auto file = HighFive::File(filepath.string(), HighFive::File::ReadWrite);

#define X(type, name, flag)                                                               \
    RAYX_VERB << "append ray attribute: " #name " (" << rays.name.size() << " elements)"; \
    if (contains(attr, RayAttrMask::flag)) {                                              \
        auto dataset        = file.getDataSet("rayx/events/" #name);                      \
        const auto old_size = dataset.getSpace().getDimensions()[0];                      \
        dataset.resize({old_size + rays.name.size()});                                    \
        dataset.select({old_size}, {rays.name.size()}).write(rays.name);                  \
    }

        RAYX_X_MACRO_RAY_ATTR
#undef X
    } catch (const std::exception& e) { RAYX_EXIT << "exception caught while attempting to write h5 file: " << e.what(); }
}

}  // namespace RAYX

#endif
