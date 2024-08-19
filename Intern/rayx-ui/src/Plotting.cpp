#include "Plotting.h"

#include "Debug/Instrumentor.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <glm.h>
#include <stb_image_write.h>

#include <array>

/**
 * This function takes a vector of rays, along with the minimum and maximum x and z coordinates,
 * and the number of cells in the x and z directions, and returns a grid representing the footprint
 * of the rays within the specified range. Each cell in the grid contains the count of rays that
 * intersect that cell.
 */
std::vector<std::vector<uint32_t>> makeFootprint(std::vector<RAYX::Ray> rays, double min_x, double max_x, double min_z, double max_z,
                                                 uint32_t cells_x, uint32_t cells_z) {
    RAYX_PROFILE_FUNCTION_STDOUT();
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

std::unique_ptr<unsigned char[]> footprintAsImage(const std::vector<std::vector<uint32_t>>& footprint, uint32_t& width, uint32_t& height) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    width = static_cast<uint32_t>(footprint.size());
    height = static_cast<uint32_t>(footprint[0].size());
    constexpr uint32_t channels = 4;  // RGBA is forced for now
    std::unique_ptr<unsigned char[]> data(new unsigned char[width * height * channels]);

    // Colors for visualizing ray density.
    const glm::vec4 low(0.0f, 0.1f, 1.0f, 1.0f);
    const glm::vec4 high(1.0f, 0.7f, 0.3f, 1.0f);
    const glm::vec4 colorDiff = high - low;

    // Find max value in a single pass
    uint32_t max = 0;
    for (const auto& row : footprint) {
        for (uint32_t value : row) {
            max = std::max(max, value);
        }
    }

    if (max == 0) max = 1;
    const float logMax = std::log10(static_cast<float>(max) + 1.0f);
    const float invLogMax = 1.0f / logMax;

    {
        constexpr int LUT_SIZE = 1024;
        std::array<glm::u8vec4, LUT_SIZE> colorLUT;

        // Initialize LUT (do this once)
        for (int i = 0; i < LUT_SIZE; ++i) {
            float normalizedValue = std::log10(static_cast<float>(i) + 1.0f) * invLogMax;
            glm::vec4 color = low + colorDiff * normalizedValue;
            colorLUT[i] = glm::u8vec4(color * 255.0f);
        }

        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                int index = std::min(static_cast<int>(footprint[x][y]), LUT_SIZE - 1);  // Correct indexing for flipping
                memcpy(&data[(y * width + x) * channels], &colorLUT[index], 4);
            }
        }
    }

    return data;
}
