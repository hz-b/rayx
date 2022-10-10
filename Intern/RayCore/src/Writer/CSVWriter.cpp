#include "CSVWriter.h"

#include <Debug.h>

#include <cstring>
#include <fstream>
#include <sstream>

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
    ss.precision(17);
    std::string s;
    ss << x;
    ss >> s;
    return strToCell(s.c_str());
}

void writeCSV(const std::vector<RAYX::Ray>& rays, std::string filename) {
    std::ofstream file(filename);

    file << strToCell("Index").buf << " | "        //
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

    int index = 0;
    for (auto ray : rays) {
        file << ulongToCell(index).buf << " | "               //
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
        index++;
    }

    RAYX_VERB << "Writing done!";
}
