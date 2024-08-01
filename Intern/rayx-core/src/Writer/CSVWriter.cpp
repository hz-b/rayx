#include "CSVWriter.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include "Debug/Debug.h"

// writer:

const int CELL_SIZE = 23;
const char DELIMITER = ',';

// The resulting CSV file consists of rows and columns. At each column-row pair, you will find a __Cell__.
// In order to make it readable, cells have a fixed size; thus the CSV file looks like a grid.
struct Cell {
    char buf[CELL_SIZE + 1];  // + 1 for null-termination.
};

using std::min;

// Tries to write a string into a cell.
// Will only write an incomplete string, if it doesn't fit.
Cell strToCell(const char* x) {
    Cell out{};
    int n = strlen(x);

    if (n > CELL_SIZE) {
        RAYX_WARN << "strToCell: string \"" << x << "\" needs to be shortened!";
    }

    for (int i = 0; i < min(n, CELL_SIZE); i++) {
        out.buf[i] = x[i];
    }
    for (int i = n; i < CELL_SIZE; i++) {
        out.buf[i] = ' ';
    }
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
    while (s.size() > CELL_SIZE) {
        s.pop_back();
    }
    return strToCell(s.c_str());
}

void writeCSV(const RAYX::BundleHistory& hist, const std::string& filename, const Format& format, int startEventID) {
    std::ofstream file(filename);

    // write the header of the CSV file:
    for (uint32_t i = 0; i < format.size(); i++) {
        if (i > 0) {
            file << DELIMITER;
        }
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
                if (i > 0) {
                    file << DELIMITER;
                }
                double d = format[i].get_double(ray_id, event_id + startEventID, event);
                file << doubleToCell(d).buf;
            }
            file << '\n';
        }
    }
    RAYX_VERB << "Writing done!";
}

// loader:

RAYX::BundleHistory loadCSV(const std::string& filename) {
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

        while (std::getline(ss, num, DELIMITER)) {
            d.push_back(std::stod(num));
        }

        if (d.size() != 16) {
            RAYX_EXIT << "CSV line has incorrect length: " << d.size();
        }

        const auto direction = glm::dvec3(d[4], d[5], d[6]);

        const auto stokes = glm::dvec4(d[8], d[9], d[10], d[11]);
        // const auto rotation = glm::transpose(RAYX::rotationMatrix(direction));
        const auto field = /* rotation *  */ RAYX::stokesToElectricField(stokes);

        // create the Ray from the loaded doubles from this line.
        RAYX::Ray ray = {.m_position = {d[0], d[1], d[2]},
                         .m_eventType = d[3],
                         .m_direction = direction,
                         .m_energy = d[7],
                         .m_field = field,
                         .m_pathLength = d[12],
                         .m_order = d[13],
                         .m_lastElement = d[14],
                         .m_sourceID = d[15]};
        // This checks whether `ray_id` is from a "new ray" that didn't yet come up in the BundleHistory.
        // If so, we need to make place for it.
        if (out.size() <= ray_id) {
            out.emplace_back();
        }

        // If the rays are out of order, we crash.
        // This happens for example if we load rays with ray-ids 0, 1, 2, 4, 3.
        // Then when the parser reads the 4, it will consider it out-of-order as it expected a 3.
        if (ray_id + 1 != out.size()) {
            RAYX_EXIT << "loadCSV failed: rays out of order";
        }
        // The event-id of the new event should match the number of previous events found for this ray.
        if (event_id != out[ray_id].size()) {
            RAYX_EXIT << "loadCSV failed: events out of order";
        }

        // put the new event into the BundleHistory.
        out[ray_id].push_back(ray);
    }

    return out;
}
