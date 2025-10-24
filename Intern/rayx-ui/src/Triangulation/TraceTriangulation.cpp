#include "TraceTriangulation.h"

#include "Beamline/Beamline.h"
#include "BundleHistory.h"
#include "Colors.h"
#include "Debug/Instrumentor.h"
#include "Shader/Collision.h"
#include "Tracer/Tracer.h"
#include "Triangulation/GeometryUtils.h"

/**
 * Given grid size, width, length, and a flag indicating the plane of the rays,
 * this function populates and returns a 2D grid of Ray objects.
 */
std::vector<std::vector<Ray>> createRayGrid(size_t size, double width, double length) {
    std::vector<std::vector<Ray>> grid(size, std::vector<Ray>(size));
    double xStep = width / size;
    double zStep = length / size;

    for (size_t i = 0; i < size; i++) {
        double x = -width / 2 + xStep * i;
        for (size_t j = 0; j < size; j++) {
            double z = -length / 2 + zStep * j;
            glm::dvec3 pos, dir;
            constexpr double distanceToObj = 2000.0f;
            pos                            = glm::dvec3(x, distanceToObj, z);
            dir                            = glm::dvec3(0.0f, -1.0f, 0.0f);
            const auto stokes              = glm::dvec4(1.0f, 0.0f, 0.0f, 0.0f);
            const auto field               = rayx::stokesToElectricFieldWithBaseConvention(stokes, dir);

            Ray ray = {
                .m_position    = pos,
                .m_eventType   = rayx::EventType::Emitted,
                .m_direction   = dir,
                .m_energy      = 1.0f,
                .m_field       = field,
                .m_pathLength  = 0.0f,
                .m_order       = 0,
                .m_lastElement = -1,
                .m_sourceID    = 0,
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
void traceTriangulation(const rayx::OpticalElement compiled, std::vector<TextureVertex>& vertices, std::vector<uint32_t>& indices) {
    using DeviceType = rayx::DeviceConfig::DeviceType;
    auto tracer      = rayx::Tracer(rayx::DeviceConfig(DeviceType::Cpu).enableBestDevice());

    constexpr size_t gridSize = 20;
    auto [width, length]      = getRectangularDimensions(compiled.m_cutout);
    BundleHistory rayGrid     = createRayGrid(gridSize, width, length);

    std::vector<std::vector<rayx::OptCollisionPoint>> collisionGrid(gridSize, std::vector<rayx::OptCollisionPoint>(gridSize, std::nullopt));

    for (size_t i = 0; i < gridSize; ++i) {
        for (size_t j = 0; j < gridSize; ++j) {
            const auto& ray = rayGrid[i][j];
            rayx::OptCollisionPoint collision =
                rayx::findCollisionInElementCoordsWithoutSlopeError(ray.m_position, ray.m_direction, compiled.m_surface, compiled.m_cutout, true);
            collisionGrid[i][j] = collision;
        }
    }

    uint32_t index = 0;

    for (size_t i = 0; i < gridSize - 1; ++i) {
        for (size_t j = 0; j < gridSize - 1; ++j) {
            if (collisionGrid[i][j] && collisionGrid[i + 1][j] && collisionGrid[i][j + 1]) {
                vertices.push_back({glm::vec4(collisionGrid[i][j]->hitpoint, 1.0f), glm::vec2(OPT_ELEMENT_COLOR)});
                vertices.push_back({glm::vec4(collisionGrid[i + 1][j]->hitpoint, 1.0f), glm::vec2(OPT_ELEMENT_COLOR)});
                vertices.push_back({glm::vec4(collisionGrid[i][j + 1]->hitpoint, 1.0f), glm::vec2(OPT_ELEMENT_COLOR)});

                indices.push_back(index++);
                indices.push_back(index++);
                indices.push_back(index++);
            }
            if (collisionGrid[i + 1][j + 1] && collisionGrid[i + 1][j] && collisionGrid[i][j + 1]) {
                vertices.push_back({glm::vec4(collisionGrid[i + 1][j + 1]->hitpoint, 1.0f), glm::vec2(OPT_ELEMENT_COLOR)});
                vertices.push_back({glm::vec4(collisionGrid[i + 1][j]->hitpoint, 1.0f), glm::vec2(OPT_ELEMENT_COLOR)});
                vertices.push_back({glm::vec4(collisionGrid[i][j + 1]->hitpoint, 1.0f), glm::vec2(OPT_ELEMENT_COLOR)});

                indices.push_back(index++);
                indices.push_back(index++);
                indices.push_back(index++);
            }
        }
    }
    if (vertices.empty() || indices.empty()) { throw std::runtime_error("Failed: Missing vertices or indices at a render object!"); }
}
