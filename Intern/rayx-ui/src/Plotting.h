#pragma once

#include <vector>

#include "Shader/Ray.h"

std::vector<std::vector<int>> makeFootprint(std::vector<RAYX::Ray> rays, double min_x, double max_x, double min_z, double max_z, int cells_x,
                                            int cells_z);

void writeFootprintAsJPG(std::vector<std::vector<int>> grid, const char* filename);

unsigned char* footprintAsJPG(std::vector<std::vector<int>> grid, int& width, int& height, int& channels);