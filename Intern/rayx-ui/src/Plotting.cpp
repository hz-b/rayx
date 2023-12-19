#include "Plotting.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

std::vector<std::vector<int>> makeFootprint(std::vector<RAYX::Ray> rays, double min_x, double max_x, double min_z, double max_z, int cells_x,
                                            int cells_z) {
    std::vector<std::vector<int>> grid;

    // init the grid.
    for (int i = 0; i < cells_x; i++) {
        std::vector<int> inner(cells_z, 0);
        grid.push_back(inner);
    }

    // fill the grid.
    for (auto r : rays) {
        double x = r.m_position.x;
        double z = r.m_position.z;

        // x2 = 0.0 means, the ray has x position min_x.
        // x2 = 1.0 means, the ray has x position max_x.
        // If x2 is between 0 and 1, it means that the x position is `min_x + (max_x - min_x) * x2`.
        // In other words, x2 is an interpolation factor between min_x and max_x.
        // We can use this factor to determine the grid cell in which to put our ray r.
        double x2 = (x - min_x) / (max_x - min_x);
        double z2 = (z - min_z) / (max_z - min_z);

        // x3 is the x-index in the grid.
        int x3 = (int)(x2 * (double)cells_x);
        int z3 = (int)(z2 * (double)cells_z);

        if (x3 >= 0 && x3 < cells_x && z3 >= 0 && z3 < cells_z) {
            grid[x3][z3]++;
        }
    }

    return grid;
}

void writeFootprintAsJPG(std::vector<std::vector<int>> grid, const char* filename) {
    int width, height, channels;
    unsigned char* data = footprintAsJPG(grid, width, height, channels);
    stbi_write_jpg(filename, width, height, channels, data, 100);
    delete[] data;
}

unsigned char* footprintAsJPG(std::vector<std::vector<int>> grid, int& width, int& height, int& channels) {
    width = (int)grid.size();
    height = (int)grid[0].size();
    channels = 3;
    unsigned char* data = new unsigned char[width * height * channels];

    for (int x = 0; x < width; x++) {
        for (int z = 0; z < height; z++) {
            int i = (x + z * width) * channels;
            int value = grid[x][z];
            data[i + 0] = (unsigned char)value;
            data[i + 1] = (unsigned char)value;
            data[i + 2] = (unsigned char)value;
        }
    }

    return data;
}