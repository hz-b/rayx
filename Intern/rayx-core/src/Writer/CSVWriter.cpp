#include "CSVWriter.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include "Debug/Debug.h"

// writer:

const int CELL_SIZE = 23;
const char DELIMITER = ',';

struct Cell {
    char buf[CELL_SIZE + 1];
};

int min(int a, int b) {
    if (a > b) {
        return b;
    }
    return a;
}

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

void writeCSV(const RAYX::BundleHistory& hist, const std::string& filename, const Format& format) {
    std::ofstream file(filename);

    // write header:
    for (uint i = 0; i < format.size(); i++) {
        if (i > 0) {
            file << DELIMITER;
        }
        file << strToCell(format[i].name).buf;
    }
    file << '\n';

    RAYX_VERB << "Writing " << hist.size() << " rays to file...";

    // write data:
    for (unsigned long ray_id = 0; ray_id < hist.size(); ray_id++) {
        const RAYX::RayHistory& ray_hist = hist[ray_id];
        for (unsigned long event_id = 0; event_id < ray_hist.size(); event_id++) {
            const RAYX::Ray& event = ray_hist[event_id];
            for (uint i = 0; i < format.size(); i++) {
                if (i > 0) {
                    file << DELIMITER;
                }
                file << doubleToCell(format[i].get_double(ray_id, event_id, event)).buf;
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
        unsigned long ray_id = std::stoi(num);

        std::getline(ss, num, DELIMITER);
        unsigned long event_id = std::stoi(num);

        while (std::getline(ss, num, DELIMITER)) {
            d.push_back(std::stod(num));
        }
        assert(d.size() == 15);
        RAYX::Ray ray = {.m_position = {d[0], d[1], d[2]},
                         .m_eventType = d[3],
                         .m_direction = {d[4], d[5], d[6]},
                         .m_energy = d[7],
                         .m_stokes = {d[8], d[9], d[10], d[11]},
                         .m_pathLength = d[12],
                         .m_order = d[13],
                         .m_lastElement = d[14],
                         .m_padding = -1.0};
        if (out.size() <= ray_id) {
            out.emplace_back();
        }
        if (ray_id + 1 != out.size()) {
            RAYX_ERR << "loadCSV failed: rays out of order";
        }
        if (event_id != out[ray_id].size()) {
            RAYX_ERR << "loadCSV failed: events out of order";
        }
        out[ray_id].push_back(ray);
    }

    return out;
}
