#include "CSVWriter.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include "Debug/Debug.h"

namespace {

constexpr char DELIMITER = ',';

}  // unnamed namespace

namespace RAYX {

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

CsvWriter::CsvWriter(const std::filesystem::path& filepath, const Format& format, int startEventIndex, int precision)
    : m_file(filepath, std::ios::out | std::ios::trunc), m_format(format), m_startEventIndex(startEventIndex), m_precision(precision) {
    RAYX_VERB << "Opening csv file \"" << filepath << "\".";

    m_file << std::setprecision(m_precision);
    m_file << std::left;

    for (uint32_t i = 0; i < m_format.size(); i++) {
        if (i > 0) {
            m_file << DELIMITER;
        }
        m_file << std::setw(m_precision + 2) << m_format[i].name;
    }
    m_file << '\n';

    validate();
}

void CsvWriter::write(const Batch& batch) {
    RAYX_VERB << "Writing " << batch.rays.size() << " rays to csv file...";

    // write single batch to the body of the CSVm_file:
    for (int ray_id = 0; ray_id < static_cast<int>(batch.rays.size()); ++ray_id) {
        const auto& ray = batch.rays[ray_id];
        for (int event_id = 0; event_id < static_cast<int>(ray.size()); ++event_id) {
            const auto& event = ray[event_id];

            for (uint32_t i = 0; i < m_format.size(); i++) {
                if (i > 0) m_file << DELIMITER;
                const auto d = m_format[i].get_double(ray_id, event_id + m_startEventIndex, event);
                m_file << std::setw(m_precision + 2) << d;
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
