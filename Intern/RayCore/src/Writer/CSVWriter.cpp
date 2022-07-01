#include <Debug.h>

#include <fstream>

#include "../Tracer/RayList.h"

void writeCSV(RAYX::RayList& rays, std::string filename) {
    std::ofstream file(filename);
    file.precision(17);

    file << "Index;Xloc;Yloc;Zloc;Weight;Xdir;Ydir;Zdir;Energy;"
            "Stokes0;Stokes1;Stokes2;Stokes3;pathLength;order;"
            "lastElement;extraParam\n";

    size_t size = rays.size();
    RAYX_D_LOG << "Writing " << size / RAY_DOUBLE_COUNT << " rays to file...";

    int index = 0;
    for (auto l : rays) {
        for (auto ray : l) {
            char buff[384];
            sprintf(
                buff,
                "%lu;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%"
                ".17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f\n",
                (unsigned long)index, ray.m_position.x, ray.m_position.y,
                ray.m_position.z, ray.m_weight, ray.m_direction.x,
                ray.m_direction.y, ray.m_direction.z, ray.m_energy,
                ray.m_stokes.x, ray.m_stokes.y, ray.m_stokes.z, ray.m_stokes.w,
                ray.m_pathLength, ray.m_order, ray.m_lastElement,
                ray.m_extraParam);
            file << buff;
            index++;
        }
    }

    RAYX_D_LOG << "Writing done!";
}
