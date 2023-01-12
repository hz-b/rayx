#include "CSVWriter.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include "Debug/Debug.h"

// writer:

const int CELL_SIZE = 23;

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

Cell ulongToCell(unsigned long x) {
    std::stringstream ss;
    std::string s;
    ss << x;
    ss >> s;
    return strToCell(s.c_str());
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

void writeCSV(const RAYX::Rays& rays, std::string filename) {
    std::ofstream file(filename);

    file << strToCell("Ray ID").buf << " | "       //
         << strToCell("Snapshot ID").buf << " | "  //
         << strToCell("X position").buf << " | "   //
         << strToCell("Y position").buf << " | "   //
         << strToCell("Z position").buf << " | "   //
         << strToCell("Weight").buf << " | "       //
         << strToCell("X direction").buf << " | "  //
         << strToCell("Y direction").buf << " | "  //
         << strToCell("Z direction").buf << " | "  //
         << strToCell("Energy").buf << " | "       //
         << strToCell("Stokes0").buf << " | "      //
         << strToCell("Stokes1").buf << " | "      //
         << strToCell("Stokes2").buf << " | "      //
         << strToCell("Stokes3").buf << " | "      //
         << strToCell("pathLength").buf << " | "   //
         << strToCell("order").buf << " | "        //
         << strToCell("lastElement").buf << " | "  //
         << strToCell("extraParam").buf << '\n';

    for (int i = 0; i < 320; i++) {
        file << '-';
    }
    file << '\n';

    RAYX_VERB << "Writing " << rays.size() << " rays to file...";

    for (unsigned long ray_id = 0; ray_id < rays.size(); ray_id++) {
        const auto& snapshots = rays[ray_id];
        for (unsigned long snapshot_id = 0; snapshot_id < snapshots.size(); snapshot_id++) {
            const auto& ray = snapshots[snapshot_id];
            file << ulongToCell(ray_id).buf << " | "              //
                 << ulongToCell(snapshot_id).buf << " | "         //
                 << doubleToCell(ray.m_position.x).buf << " | "   //
                 << doubleToCell(ray.m_position.y).buf << " | "   //
                 << doubleToCell(ray.m_position.z).buf << " | "   //
                 << doubleToCell(ray.m_weight).buf << " | "       //
                 << doubleToCell(ray.m_direction.x).buf << " | "  //
                 << doubleToCell(ray.m_direction.y).buf << " | "  //
                 << doubleToCell(ray.m_direction.z).buf << " | "  //
                 << doubleToCell(ray.m_energy).buf << " | "       //
                 << doubleToCell(ray.m_stokes.x).buf << " | "     //
                 << doubleToCell(ray.m_stokes.y).buf << " | "     //
                 << doubleToCell(ray.m_stokes.z).buf << " | "     //
                 << doubleToCell(ray.m_stokes.w).buf << " | "     //
                 << doubleToCell(ray.m_pathLength).buf << " | "   //
                 << doubleToCell(ray.m_order).buf << " | "        //
                 << doubleToCell(ray.m_lastElement).buf << " | "  //
                 << doubleToCell(ray.m_extraParam).buf << '\n';
        }
    }
    RAYX_VERB << "Writing done!";
}

// loader:

RAYX::Rays RAYX_API loadCSV(std::string filename) {
    std::ifstream file(filename);

    // ignore two setup lines
    std::string s;
    std::getline(file, s);
    std::getline(file, s);

    RAYX::Rays out;

    while (std::getline(file, s)) {
        std::vector<double> d;
        std::stringstream ss(s);
        std::string num;

        std::getline(ss, num, '|');
        unsigned long ray_id = std::stoi(num);

        std::getline(ss, num, '|');
        unsigned long snapshot_id = std::stoi(num);

        while (std::getline(ss, num, '|')) {
            d.push_back(std::stod(num));
        }
        assert(d.size() == 16);
        RAYX::Ray ray = {.m_position = {d[0], d[1], d[2]},
                         .m_weight = d[3],
                         .m_direction = {d[4], d[5], d[6]},
                         .m_energy = d[7],
                         .m_stokes = {d[8], d[9], d[10], d[11]},
                         .m_pathLength = d[12],
                         .m_order = d[13],
                         .m_lastElement = d[14],
                         .m_extraParam = d[15]};
        if (out.size() >= ray_id) {
            out.push_back({});
        }
        assert(ray_id + 1 == out.size());
        assert(snapshot_id == out[ray_id].size());
        out[ray_id].push_back(ray);
    }

    return out;
}
