#include "TraceTriangulation.h"

#include "Beamline/Beamline.h"
#include "Colors.h"
#include "Tracer/CpuTracer.h"

namespace RAYX {
namespace CPU_TRACER {
#include "Shared/Collision.h"
#include "Shared/Element.h"  // I think this contains Surface & Cutout
#include "Shared/Ray.h"
RAYX_API Collision findCollisionInElementCoords(Ray, Surface, Cutout);
}  // namespace CPU_TRACER
}  // namespace RAYX

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

std::vector<RenderObject> traceTriangulation(const std::vector<RAYX::OpticalElement>& elements) {
    RAYX::CpuTracer tracer;
    std::vector<RenderObject> objects;

    for (const RAYX::OpticalElement& element : elements) {
        size_t size = 10;
        auto [width, height] = getRectangularDimensions(element.m_element.m_cutout);
        bool isXZ = element.m_element.m_behaviour.m_type == BTYPE_SLIT || element.m_element.m_behaviour.m_type == BTYPE_SLIT;
        auto rayGrid = createRayGrid(size, width, height, isXZ);

        std::vector<std::vector<bool>> grid(size, std::vector<bool>(size, false));

        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                RAYX::CPU_TRACER::Collision coll =
                    RAYX::CPU_TRACER::findCollisionInElementCoords(rayGrid[i][j], element.m_element.m_surface, element.m_element.m_cutout);

                if (coll.found) {
                    grid[i][j] = true;
                }
            }
        }

        // triangulate over grid
        std::vector<Triangle> triangles;
        for (size_t i = 0; i < size - 1; i++) {
            for (size_t j = 0; j < size - 1; j++) {
                if (grid[i][j] && grid[i + 1][j] && grid[i][j + 1]) {
                    Vertex v1 = {glm::vec4(rayGrid[i][j].m_position, 1.0f), DARKER_BLUE};
                    Vertex v2 = {glm::vec4(rayGrid[i + 1][j].m_position, 1.0f), BLUE};
                    Vertex v3 = {glm::vec4(rayGrid[i][j + 1].m_position, 1.0f), BLUE};
                    triangles.push_back(Triangle(v1, v2, v3));
                }
                if (grid[i + 1][j + 1] && grid[i + 1][j] && grid[i][j + 1]) {
                    Vertex v1 = {glm::vec4(rayGrid[i + 1][j + 1].m_position, 1.0f), BLUE};
                    Vertex v2 = {glm::vec4(rayGrid[i + 1][j].m_position, 1.0f), BLUE};
                    Vertex v3 = {glm::vec4(rayGrid[i][j + 1].m_position, 1.0f), LIGHTER_BLUE};
                    triangles.push_back(Triangle(v1, v2, v3));
                }
            }
        }

        // add vertices to render object
        RenderObject object(element.m_element.m_outTrans);  // Only one element per beamline
        for (const Triangle& triangle : triangles) {
            object.addTriangle(triangle);
        }
        objects.push_back(object);
        std::cout << "Added " << triangles.size() << " triangles to new render object" << std::endl;
    }

    return objects;
}