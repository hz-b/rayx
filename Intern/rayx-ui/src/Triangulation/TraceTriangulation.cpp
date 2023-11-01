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
 * length depending on the cutout's type (rectangle, ellipse, trapezoid, etc.).
 */
std::pair<double, double> getRectangularDimensions(const Cutout& cutout) {
    double width = 0.0;
    double length = 0.0;

    switch (static_cast<int>(cutout.m_type)) {
        case CTYPE_RECT: {
            RectCutout rect = deserializeRect(cutout);
            width = rect.m_width;
            length = rect.m_length;
            break;
        }
        case CTYPE_ELLIPTICAL: {
            EllipticalCutout ell = deserializeElliptical(cutout);
            width = ell.m_diameter_x;   // Diameter is essentially the max width
            length = ell.m_diameter_z;  // Diameter is the max length
            break;
        }
        case CTYPE_TRAPEZOID: {
            TrapezoidCutout trap = deserializeTrapezoid(cutout);
            width = std::max(trap.m_widthA, trap.m_widthB);  // max of the two sides
            length = trap.m_length;
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

    return {width, length};
}

/**
 * Given grid size, width, length, and a flag indicating the plane of the rays,
 * this function populates and returns a 2D grid of RAYX::Ray objects.
 */
std::vector<std::vector<RAYX::Ray>> createRayGrid(size_t size, double width, double length) {
    std::vector<std::vector<RAYX::Ray>> grid(size, std::vector<RAYX::Ray>(size));
    double xStep = width / size;
    double zStep = length / size;

    for (size_t i = 0; i < size; i++) {
        double x = -width / 2 + xStep * i;
        for (size_t j = 0; j < size; j++) {
            double z = -length / 2 + zStep * j;
            glm::dvec3 pos, dir;
            pos = glm::dvec3(x, 0.0f, z);
            dir = glm::dvec3(0.0f, 1.0f, 0.0f);

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
    auto [width, length] = getRectangularDimensions(element.m_element.m_cutout);
    auto rayGrid = createRayGrid(gridSize, width, length);

    std::vector<std::vector<bool>> collisionGrid(gridSize, std::vector<bool>(gridSize, false));

    for (size_t i = 0; i < gridSize; ++i) {
        for (size_t j = 0; j < gridSize; ++j) {
            auto collision =
                RAYX::CPU_TRACER::findCollisionInElementCoords(rayGrid[i][j], element.m_element.m_surface, element.m_element.m_cutout, true);
            collisionGrid[i][j] = collision.found;
        }
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t index = 0;

    for (size_t i = 0; i < gridSize - 1; ++i) {
        for (size_t j = 0; j < gridSize - 1; ++j) {
            if (collisionGrid[i][j] && collisionGrid[i + 1][j] && collisionGrid[i][j + 1]) {
                vertices.emplace_back(glm::vec4(rayGrid[i][j].m_position, 1.0f), DARKER_OPT_ELEMENT_COLOR);
                vertices.emplace_back(glm::vec4(rayGrid[i + 1][j].m_position, 1.0f), OPT_ELEMENT_COLOR);
                vertices.emplace_back(glm::vec4(rayGrid[i][j + 1].m_position, 1.0f), OPT_ELEMENT_COLOR);

                indices.push_back(index++);
                indices.push_back(index++);
                indices.push_back(index++);
            }
            if (collisionGrid[i + 1][j + 1] && collisionGrid[i + 1][j] && collisionGrid[i][j + 1]) {
                vertices.emplace_back(glm::vec4(rayGrid[i + 1][j + 1].m_position, 1.0f), OPT_ELEMENT_COLOR);
                vertices.emplace_back(glm::vec4(rayGrid[i + 1][j].m_position, 1.0f), OPT_ELEMENT_COLOR);
                vertices.emplace_back(glm::vec4(rayGrid[i][j + 1].m_position, 1.0f), LIGHTER_OPT_ELEMENT_COLOR);

                indices.push_back(index++);
                indices.push_back(index++);
                indices.push_back(index++);
            }
        }
    }
    if (vertices.empty() || indices.empty()) {
        throw std::runtime_error("Failed: Missing vertices or indices at a render object!");
    }
    RenderObject renderObj(element.m_name, device, element.m_element.m_outTrans, vertices, indices);
    std::cout << "Added " << vertices.size() / 3 << " triangles to new render object" << std::endl;

    return renderObj;
}
