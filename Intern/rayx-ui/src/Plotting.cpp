#include "Plotting.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <glm/glm.hpp>

/**
 * This function takes a vector of rays, along with the minimum and maximum x and z coordinates,
 * and the number of cells in the x and z directions, and returns a grid representing the footprint
 * of the rays within the specified range. Each cell in the grid contains the count of rays that
 * intersect that cell.
 */
std::vector<std::vector<uint32_t>> makeFootprint(std::vector<RAYX::Ray> rays, double min_x, double max_x, double min_z, double max_z,
                                                 uint32_t cells_x, uint32_t cells_z) {
    std::vector<std::vector<uint32_t>> grid;
    assert(min_x < max_x && min_z < max_z && cells_x > 0 && cells_z > 0);

    // init the grid.
    for (uint32_t i = 0; i < cells_x; i++) {
        std::vector<uint32_t> inner(cells_z, 0);
        grid.push_back(inner);
    }

    // fill the grid.
    for (auto r : rays) {
        double x = -r.m_position.x;  // TODO: Figure out why this needs to be negated.
        double z = -r.m_position.z;

        // x2 = 0.0 means, the ray has x position min_x.
        // x2 = 1.0 means, the ray has x position max_x.
        // If x2 is between 0 and 1, it means that the x position is `min_x + (max_x - min_x) * x2`.
        // In other words, x2 is an interpolation factor between min_x and max_x.
        // We can use this factor to determine the grid cell in which to put our ray r.
        double x2 = (x - min_x) / (max_x - min_x);
        double z2 = (z - min_z) / (max_z - min_z);
        if (x2 < 0.0 || x2 > 1.0 || z2 < 0.0 || z2 > 1.0) continue;

        // x3 is the x-index in the grid.
        uint32_t x3 = (uint32_t)(x2 * (double)cells_x);
        uint32_t z3 = (uint32_t)(z2 * (double)cells_z);
        grid[x3][z3]++;
    }

    return grid;
}

void dumpFootprint(std::vector<std::vector<uint32_t>> footprint) {
    for (auto row : footprint) {
        for (auto cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<std::vector<uint32_t>> makeFootprintSquare(std::vector<RAYX::Ray> rays, double min_x, double max_x, uint32_t cells_x) {
    return makeFootprint(rays, min_x, max_x, min_x, max_x, cells_x, cells_x);
}

void writeFootprintAsPNG(std::vector<std::vector<uint32_t>> footprint, const char* filename) {
    uint32_t width, height;
    constexpr uint32_t channels = 4;  // RGBA is forced for now
    std::unique_ptr<unsigned char[]> data = footprintAsImage(footprint, width, height);
    stbi_write_png(filename, width, height, channels, data.get(), width * channels);
}

std::unique_ptr<unsigned char[]> footprintAsImage(std::vector<std::vector<uint32_t>> footprint, uint32_t& width, uint32_t& height) {
    width = static_cast<uint32_t>(footprint.size());
    height = static_cast<uint32_t>(footprint[0].size());
    constexpr uint32_t channels = 4;  // RGBA is forced for now
    std::unique_ptr<unsigned char[]> data(new unsigned char[width * height * channels]);

    // Colors for visualizing ray density.
    glm::vec4 low = glm::vec4(0.0, 0.1, 1.0, 1.0);
    glm::vec4 high = glm::vec4(1.0, 0.7, 0.3, 1.0);

    uint32_t max = 0;
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t z = 0; z < height; z++) {
            uint32_t value = footprint[x][z];
            if (value > max) max = value;
        }
    }

    if (max == 0) max = 1;

    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t z = 0; z < height; z++) {
            uint32_t value = footprint[x][z];
            float logValue = std::log10(static_cast<float>(value) + 1.0f);
            float logMax = std::log10(static_cast<float>(max) + 1.0f);
            float normalizedValue = logValue / logMax;
            glm::vec4 color = glm::mix(low, high, normalizedValue);
            uint32_t index = (x + z * width) * channels;
            data[index + 0] = static_cast<unsigned char>(color.r * 255.0);
            data[index + 1] = static_cast<unsigned char>(color.g * 255.0);
            data[index + 2] = static_cast<unsigned char>(color.b * 255.0);
            data[index + 3] = static_cast<unsigned char>(color.a * 255.0);
        }
    }

    return data;
}