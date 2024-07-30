#pragma once

#include <memory>
#include <vector>

#include "Shader/Ray.h"

/**
 * @brief Creates a grid representing the footprint of rays within a specified range.
 *
 * This function takes a vector of rays, along with the minimum and maximum x and z coordinates,
 * and the number of cells in the x and z directions, and returns a grid representing the footprint
 * of the rays within the specified range. Each cell in the grid contains the count of rays that
 * intersect that cell.
 *
 * @param rays The vector of rays.
 * @param min_x The minimum x coordinate.
 * @param max_x The maximum x coordinate.
 * @param min_z The minimum z coordinate.
 * @param max_z The maximum z coordinate.
 * @param cells_x The number of cells in the x direction.
 * @param cells_z The number of cells in the z direction.
 * @return The grid representing the footprint of the rays.
 */
std::vector<std::vector<uint32_t>> makeFootprint(std::vector<RAYX::Ray> rays, double min_x, double max_x, double min_z, double max_z,
                                                 uint32_t cells_x, uint32_t cells_z);

void dumpFootprint(std::vector<std::vector<uint32_t>> footprint);

/**
 * @brief Square version of makeFootprint.
 *
 * @see makeFootprint
 */
std::vector<std::vector<uint32_t>> makeFootprintSquare(std::vector<RAYX::Ray> rays, double min_x, double max_x, uint32_t cells_x);

/**
 * @brief Writes a footprint to a JPG file.
 *
 * @param footprint The footprint to write.
 * @param filename The filename to write to.
 */
void writeFootprintAsPNG(std::vector<std::vector<uint32_t>> footprint, const char* filename);

/**
 * @brief Creates a JPG image data from a footprint.
 *
 * @param footprint The footprint to convert.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image (3 for RGB).
 * @return The image data.
 */
std::unique_ptr<unsigned char[]> footprintAsImage(const std::vector<std::vector<uint32_t>>& footprint, uint32_t& width, uint32_t& height);
