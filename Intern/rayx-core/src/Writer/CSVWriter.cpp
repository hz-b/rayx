#include "CSVWriter.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include "Debug/Debug.h"

namespace {

const int CELL_SIZE = 23;
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

    if (n > CELL_SIZE) {
        RAYX_WARN << "strToCell: string \"" << x << "\" needs to be shortened!";
    }

    for (int i = 0; i < std::min(n, CELL_SIZE); i++) {
        out.buf[i] = x[i];
    }
    for (int i = n; i < CELL_SIZE; i++) {
        out.buf[i] = ' ';
    }
    out.buf[CELL_SIZE - 1] = '\0';
    return out;
}

// TODO: can we optimize ?
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

}  // unnamed namespace

namespace RAYX {

void RAYX_API writeCSV(const BundleHistory& hist, const std::string& filename, const Format& format, int startEventID) {}

BundleHistory loadCSV(const std::string& filename) {
    std::ifstream file(filename);

    // ignore setup line
    std::string s;
    std::getline(file, s);

    BundleHistory out;

    while (std::getline(file, s)) {
        std::vector<double> d;
        std::stringstream ss(s);
        std::string num;

        std::getline(ss, num, DELIMITER);
        unsigned long ray_id = std::stoi(num);

        std::getline(ss, num, DELIMITER);
        unsigned long event_id = std::stoi(num);

        while (std::getline(ss, num, DELIMITER)) {
            d.push_back(std::stod(num));
        }

        if (d.size() != 16) {
            RAYX_ERR << "CSV line has incorrect length: " << d.size();
        }

        const auto direction = glm::dvec3(d[4], d[5], d[6]);

        const auto stokes = glm::dvec4(d[8], d[9], d[10], d[11]);
        // const auto rotation = glm::transpose(rotationMatrix(direction));
        const auto field = /* rotation *  */ stokesToElectricField(stokes);

        // create the Ray from the loaded doubles from this line.
        Ray ray = {.m_position = {d[0], d[1], d[2]},
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
            RAYX_ERR << "loadCSV failed: rays out of order";
        }
        // The event-id of the new event should match the number of previous events found for this ray.
        if (event_id != out[ray_id].size()) {
            RAYX_ERR << "loadCSV failed: events out of order";
        }

        // put the new event into the BundleHistory.
        out[ray_id].push_back(ray);
    }

    return out;
}

CsvWriter::CsvWriter(const std::filesystem::path& filepath, const Format& format, int startEventIndex)
    : m_file(filepath, std::ios::out | std::ios::trunc), m_format(format), m_startEventIndex(startEventIndex) {
    for (uint32_t i = 0; i < m_format.size(); i++) {
        if (i > 0) {
            m_file << DELIMITER;
        }
        m_file << strToCell(m_format[i].name).buf;
    }
    m_file << '\n';

    validate();
}

// TODO: can we optimize ?
void CsvWriter::write(const DeviceTracer::BatchOutput& batch) {
    RAYX_VERB << "Writing " << batch.eventCounts.size() << " rays to csvm_file...";

    // write single batch to the body of the CSVm_file:
    for (size_t ray_id = 0; ray_id < batch.eventCounts.size(); ++ray_id) {
        const auto offset = batch.eventOffsets[ray_id];
        const auto count = batch.eventCounts[ray_id];

        for (int event_id = 0; event_id < count; ++event_id) {
            const auto& event = batch.events[offset + event_id];

            for (uint32_t i = 0; i < m_format.size(); i++) {
                if (i > 0) m_file << DELIMITER;
                const auto d = m_format[i].get_double(ray_id, event_id + m_startEventIndex, event);
                m_file << doubleToCell(d).buf;
            }
            m_file << '\n';
        }
    }

    validate();
}

void CsvWriter::validate() {
    if (!m_file) {
        if (m_file.rdstate() == std::ios_base::failbit)
            RAYX_ERR << "formatting error";
        else
            RAYX_ERR << "unknowne error";
    }
}

}  // namespace RAYX
