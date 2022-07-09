#include "CSVWriter.hpp"

#include <Debug.h>

#include "../Tracer/RayList.h"

#define SHORTOUTPUT false

CSVWriter::CSVWriter() : m_outputFile("output.csv") {
    m_outputFile.precision(17);
    if (SHORTOUTPUT)
        m_outputFile << "Index;Xloc;Yloc\n";
    else
        m_outputFile << "Index;Xloc;Yloc;Zloc;Weight;Xdir;Ydir;Zdir;Energy;"
                        "Stokes0;Stokes1;Stokes2;Stokes3;pathLength;order;"
                        "lastElement;extraParam\n";
}

CSVWriter::~CSVWriter() {}

void CSVWriter::appendRays(const std::vector<double>& outputRays,
                           size_t index) {
    size_t size = outputRays.size();

    RAYX_D_LOG << "Writing " << outputRays.size() / RAY_DOUBLE_COUNT
               << " rays to file...";

    if (SHORTOUTPUT) {
        char buff[64];
        for (size_t i = 0; i < size; i = i + RAY_DOUBLE_COUNT) {
            sprintf(buff, "%lu;%.17f;%.17f\n", (unsigned long)index,
                    outputRays[i], outputRays[i + 1]);
            m_outputFile << buff;
            index++;
        }
    } else {
        char buff[384];
        for (size_t i = 0; i < size; i = i + RAY_DOUBLE_COUNT) {
            sprintf(
                buff,
                "%lu;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%"
                ".17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f\n",
                (unsigned long)index, outputRays[i], outputRays[i + 1],
                outputRays[i + 2], outputRays[i + 3], outputRays[i + 4],
                outputRays[i + 5], outputRays[i + 6], outputRays[i + 7],
                outputRays[i + 8], outputRays[i + 9], outputRays[i + 10],
                outputRays[i + 11], outputRays[i + 12], outputRays[i + 13],
                outputRays[i + 14], outputRays[i + 15]);
            m_outputFile << buff;
            index++;
        }
    }

    RAYX_D_LOG << "Writing done!";
}