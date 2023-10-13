#include "TraceTriangulation.h"

#include "Beamline/Beamline.h"
#include "Colors.h"
#include "Tracer/CpuTracer.h"

namespace RAYX {
namespace CPU_TRACER {
#include "Shared/Collision.h"
#include "Shared/Element.h"
#include "Shared/Ray.h"
RAYX_API Collision findCollisionInElementCoords(Ray, Surface, Cutout, bool);
}  // namespace CPU_TRACER
}  // namespace RAYX

/**
 * Given a Cutout object, this function calculates and returns the width and
 * height depending on the cutout's type (rectangle, ellipse, trapezoid, etc.).
 */
std::pair<double, double> getRectangularDimensions(const Cutout& cutout) {
    double width = 0.0;
    double height = 0.0;

    switch (static_cast<int>(cutout.m_type)) {
        case CTYPE_RECT: {
            RectCutout rect = deserializeRect(cutout);
            width = rect.m_size_x1;
            height = rect.m_size_x2;
            break;
        }
        case CTYPE_ELLIPTICAL: {
            width = cutout.m_params[0];   // Diameter is essentially the max width
            height = cutout.m_params[1];  // Diameter is the max height
            break;
        }
        case CTYPE_TRAPEZOID: {
            width = std::max(cutout.m_params[0], cutout.m_params[1]);  // max of the two sides
            height = cutout.m_params[2];
            break;
        }
        // Skip unlimited cutout
        case CTYPE_UNLIMITED: {
            break;
        }
        default:
            // TODO
            break;
    }

    return {width, height};
}

/**
 * Given grid size, width, height, and a flag indicating the plane of the rays,
 * this function populates and returns a 2D grid of RAYX::Ray objects.
 */
std::vector<std::vector<RAYX::Ray>> createRayGrid(size_t size, double width, double height, bool isXZ) {
    std::vector<std::vector<RAYX::Ray>> grid(size, std::vector<RAYX::Ray>(size));
    double xStep = width / size;
    double yStep = height / size;

    for (size_t i = 0; i < size; i++) {
        double x = -width / 2 + xStep * i;
        for (size_t j = 0; j < size; j++) {
            double y = -height / 2 + yStep * j;
            glm::dvec3 pos, dir;
            if (!isXZ) {
                pos = glm::dvec3(x, -2.0f, y);
                dir = glm::dvec3(0.0f, 1.0f, 0.0f);
            } else {
                pos = glm::dvec3(x, y, -2.0f);
                dir = glm::dvec3(0.0f, 0.0f, 1.0f);
            }

            RAYX::Ray ray = {
                .m_position = pos,
                .m_eventType = ETYPE_UNINIT,
                .m_direction = dir,
                .m_energy = 1.0f,
                .m_stokes = glm::dvec4(1.0f, 0.0f, 0.0f, 0.0f),
                .m_pathLength = 0.0f,
                .m_order = 0.0f,
                .m_lastElement = -1.0f,
                .m_padding = 0.0f,
            };
            grid[i][j] = ray;
        }
    }

    return grid;
}

/**
 * This function takes an OpticalElement and a Device object as input. It employs a grid of rays, created based on the dimensions of the element's
 * cutout. Using CPU-based ray tracing, it computes the intersections between rays and the optical element's surface within the cutout. The ray
 * intersections are then grouped into triangles based on the grid, and a RenderObject representing these triangles is returned.
 */
RenderObject traceTriangulation(const RAYX::OpticalElement& element, Device& device) {
    RAYX::CpuTracer tracer;

    const size_t gridSize = 10;
    auto [width, height] = getRectangularDimensions(element.m_element.m_cutout);
    const bool isXZ = element.m_element.m_behaviour.m_type == BTYPE_SLIT;
    auto rayGrid = createRayGrid(gridSize, width, height, isXZ);

    std::vector<std::vector<bool>> collisionGrid(gridSize, std::vector<bool>(gridSize, false));

    for (size_t i = 0; i < gridSize; ++i) {
        for (size_t j = 0; j < gridSize; ++j) {
            auto collision = RAYX::CPU_TRACER::findCollisionInElementCoords(rayGrid[i][j], element.m_element.m_surface, element.m_element.m_cutout,true);
            collisionGrid[i][j] = collision.found;
        }
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t index = 0;

    for (size_t i = 0; i < gridSize - 1; ++i) {
        for (size_t j = 0; j < gridSize - 1; ++j) {
            if (collisionGrid[i][j] && collisionGrid[i + 1][j] && collisionGrid[i][j + 1]) {
                vertices.emplace_back(glm::vec4(rayGrid[i][j].m_position, 1.0f), DARKER_BLUE);
                vertices.emplace_back(glm::vec4(rayGrid[i + 1][j].m_position, 1.0f), BLUE);
                vertices.emplace_back(glm::vec4(rayGrid[i][j + 1].m_position, 1.0f), BLUE);

                indices.push_back(index++);
                indices.push_back(index++);
                indices.push_back(index++);
            }
            if (collisionGrid[i + 1][j + 1] && collisionGrid[i + 1][j] && collisionGrid[i][j + 1]) {
                vertices.emplace_back(glm::vec4(rayGrid[i + 1][j + 1].m_position, 1.0f), BLUE);
                vertices.emplace_back(glm::vec4(rayGrid[i + 1][j].m_position, 1.0f), BLUE);
                vertices.emplace_back(glm::vec4(rayGrid[i][j + 1].m_position, 1.0f), LIGHTER_BLUE);

                indices.push_back(index++);
                indices.push_back(index++);
                indices.push_back(index++);
            }
        }
    }
    if (vertices.empty() || indices.empty()) {
        throw std::runtime_error("Failed: Missing vertices or indices at a render object!");
    }
    RenderObject renderObj(device, element.m_element.m_outTrans, vertices, indices);
    std::cout << "Added " << vertices.size() / 3 << " triangles to new render object" << std::endl;

    return renderObj;
}
