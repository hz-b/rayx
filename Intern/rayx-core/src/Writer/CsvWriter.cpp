#include "CsvWriter.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include "Debug/Debug.h"

namespace {

// writer:

const int CELL_SIZE  = 23;
const char DELIMITER = ',';

// The resulting CSV file consists of rows and columns. At each column-row pair, you will find a __Cell__.
// In order to make it readable, cells have a fixed size; thus the CSV file looks like a grid.
struct Cell {
    char buf[CELL_SIZE + 1];  // + 1 for null-termination.
};

// Tries to write a string into a cell.
// Will only write an incomplete string, if it doesn't fit.
Cell strToCell(const char* x) {
    Cell out{};
    int n = strlen(x);

    if (n > CELL_SIZE) { RAYX_WARN << "strToCell: string \"" << x << "\" needs to be shortened!"; }

    for (int i = 0; i < std::min(n, CELL_SIZE); i++) { out.buf[i] = x[i]; }
    for (int i = n; i < CELL_SIZE; i++) { out.buf[i] = ' '; }
    out.buf[CELL_SIZE - 1] = '\0';
    return out;
}

// Formats a double into a cell.
Cell doubleToCell(double x) {
    std::stringstream ss;
    ss.setf(std::ios::fixed);

    ss.precision(CELL_SIZE);
    std::string s;
    ss << x;
    ss >> s;

    // remove digits which do not fit.
    while (s.size() > CELL_SIZE) { s.pop_back(); }
    return strToCell(s.c_str());
}

// When writing a Ray to CSV / H5, one needs to specify a way in which to format the ray.
// A format consists of multiple components - each component corresponds to a single double of data.
// Example: "Ray-ID|Event-ID". This simple format has two components.
// A ray formatted with this format only stores its ray-id and its event-id.
struct FormatComponent {
    // The name of the component, example: "X-position".
    const char* name;
    // A function pointer expressing how to access this component given an actual RAYX::Ray.
    double (*get_double)(uint32_t ray_id, uint32_t event_id, RAYX::Ray ray);
};

// Again, a format is simply a list of components!
using Format = std::vector<FormatComponent>;

// The "full" format, consisting of all components that rays support.
// All other formats are derived by picking a subset of these components, and potentially reordering them.
static Format FULL_FORMAT = {
    FormatComponent{
        .name       = "Ray-ID",
        .get_double = [](uint32_t ray_id, uint32_t, RAYX::Ray) { return (double)ray_id; },
    },
    FormatComponent{
        .name       = "Event-ID",
        .get_double = [](uint32_t, uint32_t event_id, RAYX::Ray) { return (double)event_id; },
    },
    FormatComponent{
        .name       = "X-position",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_position.x; },
    },
    FormatComponent{
        .name       = "Y-position",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_position.y; },
    },
    FormatComponent{
        .name       = "Z-position",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_position.z; },
    },
    FormatComponent{
        .name       = "Event-type",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return static_cast<double>(ray.m_eventType); },
    },
    FormatComponent{
        .name       = "X-direction",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_direction.x; },
    },
    FormatComponent{
        .name       = "Y-direction",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_direction.y; },
    },
    FormatComponent{
        .name       = "Z-direction",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_direction.z; },
    },
    FormatComponent{
        .name       = "Energy",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_energy; },
    },
    FormatComponent{
        .name       = "ElectricField-x-real",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_field.x.real(); },
    },
    FormatComponent{
        .name       = "ElectricField-x-imag",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_field.x.imag(); },
    },
    FormatComponent{
        .name       = "ElectricField-y-real",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_field.y.real(); },
    },
    FormatComponent{
        .name       = "ElectricField-y-imag",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_field.y.imag(); },
    },
    FormatComponent{
        .name       = "ElectricField-z-real",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_field.z.real(); },
    },
    FormatComponent{
        .name       = "ElectricField-z-imag",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_field.z.imag(); },
    },
    FormatComponent{
        .name       = "pathLength",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return ray.m_pathLength; },
    },
    FormatComponent{
        .name       = "order",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return static_cast<double>(ray.m_order); },
    },
    FormatComponent{
        .name       = "lastElement",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return static_cast<double>(ray.m_lastElement); },
    },
    FormatComponent{
        .name       = "lightSourceIndex",
        .get_double = [](uint32_t, uint32_t, RAYX::Ray ray) { return static_cast<double>(ray.m_sourceID); },
    },
};

}  // unnamed namespace

namespace RAYX {

void writeCsv(const RAYX::BundleHistory& hist, const std::string& filename) {
    std::ofstream file(filename);

    const auto format = FULL_FORMAT;

    // write the header of the CSV file:
    for (uint32_t i = 0; i < format.size(); i++) {
        if (i > 0) { file << DELIMITER; }
        file << strToCell(format[i].name).buf;
    }
    file << '\n';

    RAYX_VERB << "Writing " << hist.size() << " rays to file...";

    // write the body of the CSV file:
    for (uint64_t ray_id = 0; ray_id < hist.size(); ray_id++) {
        const RAYX::RayHistory& ray_hist = hist[ray_id];
        for (size_t event_id = 0; event_id < ray_hist.size(); event_id++) {
            const RAYX::Ray& event = ray_hist[event_id];
            for (uint32_t i = 0; i < format.size(); i++) {
                if (i > 0) { file << DELIMITER; }
                double d = format[i].get_double(static_cast<uint32_t>(ray_id), static_cast<int>(event_id), event);
                file << doubleToCell(d).buf;
            }
            file << '\n';
        }
    }
    RAYX_VERB << "Writing done!";
}

RAYX::BundleHistory loadCsv(const std::string& filename) {
    std::ifstream file(filename);

    // ignore setup line
    std::string s;
    std::getline(file, s);

    RAYX::BundleHistory out;

    while (std::getline(file, s)) {
        std::vector<double> d;
        std::stringstream ss(s);
        std::string num;

        std::getline(ss, num, DELIMITER);
        uint64_t ray_id = std::stoi(num);

        std::getline(ss, num, DELIMITER);
        uint64_t event_id = std::stoi(num);

        while (std::getline(ss, num, DELIMITER)) { d.push_back(std::stod(num)); }

        if (d.size() != 18) RAYX_EXIT << "CSV line has incorrect length: " << d.size() << ". should be 18";

        int o = 0;

        const auto rayPosition = glm::dvec3(d[o], d[o + 1], d[o + 2]);
        o += 3;

        const auto rayEventType = static_cast<EventType>(d[o]);
        o += 1;

        const auto rayDirection = glm::dvec3(d[o], d[o + 1], d[o + 2]);
        o += 3;

        const auto rayEnergy = d[o];
        o += 1;

        const auto rayElectricField = ElectricField{{d[o], d[o + 1]}, {d[o + 2], d[o + 3]}, {d[o + 4], d[o + 5]}};
        o += 6;

        const auto rayPathLength = d[o];
        o += 1;

        const auto rayOrder = static_cast<Order>(d[o]);
        o += 1;

        const auto rayElementId = static_cast<ElementId>(d[o]);
        o += 1;

        const auto rayint = static_cast<int>(d[o]);
        o += 1;

        assert(o == 18);

        // create the Ray from the loaded doubles from this line.
        RAYX::Ray ray = {
            .m_position    = rayPosition,
            .m_eventType   = rayEventType,
            .m_direction   = rayDirection,
            .m_energy      = rayEnergy,
            .m_field       = rayElectricField,
            .m_pathLength  = rayPathLength,
            .m_order       = rayOrder,
            .m_lastElement = rayElementId,
            .m_sourceID    = raySourceId,
        };

        // This checks whether `ray_id` is from a "new ray" that didn't yet come up in the BundleHistory.
        // If so, we need to make place for it.
        if (out.size() <= ray_id) { out.emplace_back(); }

        // If the rays are out of order, we crash.
        // This happens for example if we load rays with ray-ids 0, 1, 2, 4, 3.
        // Then when the parser reads the 4, it will consider it out-of-order as it expected a 3.
        if (ray_id + 1 != out.size()) { RAYX_EXIT << "loadCSV failed: rays out of order"; }
        // The event-id of the new event should match the number of previous events found for this ray.
        if (event_id != out[ray_id].size()) { RAYX_EXIT << "loadCSV failed: events out of order"; }

        // put the new event into the BundleHistory.
        out[ray_id].push_back(ray);
    }

    return out;
}

}  // namespace RAYX
