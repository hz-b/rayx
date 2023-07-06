#include "Scene.h"

#include <random>

#include "Debug/Debug.h"

// TODO(Jannis): Change this so handling the triangles and lines is easier. Avoiding duplicate vertices is currently unneccessary.
Scene::Scene() {
    addTriangle({{-0.5f, 0.5f, -0.5f}, {0.1f, 0.1f, 0.1f}}, {{-0.5f, -0.5f, 0.5f}, {0.1f, 0.1f, 0.1f}}, {{0.5f, -0.5f, 0.5f}, {0.1f, 0.1f, 0.1f}});
    addTriangle({{0.5f, 0.5f, -0.5f}, {0.1f, 0.1f, 0.1f}}, {{-0.5f, 0.5f, -0.5f}, {0.1f, 0.1f, 0.1f}}, {{0.5f, -0.5f, 0.5f}, {0.1f, 0.1f, 0.1f}});
    addLine({{0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}});
    addLine({{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}, {{0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}});
}

Scene::Scene(const RAYX::RenderObjectVec& renderObjects, const RAYX::BundleHistory& bundleHistory) {
    for (const auto& renderObject : renderObjects) {
        fromRenderObject(renderObject);
    }

    for (const auto& bundle : bundleHistory) {
        for (const auto& intersection : bundle) {
            glm::vec3 yellow = {1.0f, 1.0f, 0.0f};
            Vertex origin = {{0.0f, 0.0f, 0.0f}, yellow};  // ! Only one bounce is supported for now
            Vertex point = {{intersection.m_position.x, intersection.m_position.y, intersection.m_position.z}, yellow};
            addLine(origin, point);
        }
    }
}

void Scene::addTriangle(const Vertex v1, const Vertex v2, const Vertex v3) {
    addVertex(v1, TRIA_TOPOGRAPHY);
    addVertex(v2, TRIA_TOPOGRAPHY);
    addVertex(v3, TRIA_TOPOGRAPHY);
}

void Scene::addLine(const Vertex v1, const Vertex v2) {
    addVertex(v1, LINE_TOPOGRAPHY);
    addVertex(v2, LINE_TOPOGRAPHY);
}

void Scene::fromRenderObject(const RAYX::RenderObject& renderObject) {
    if (renderObject.cutout.m_type != CTYPE_RECT) {
        RAYX_ERR << "Unsupported cutout type!";
    }

    auto rect = deserializeRect(renderObject.cutout);
    auto width = rect.m_size_x1;
    auto height = rect.m_size_x2;

    // Calculate two triangle from the position, orientation and size of the render object
    // Assume the rectangle is in the XZ plane, and its center is at position.

    // First, calculate the corners of the rectangle in the model's local space
    glm::vec4 topLeft(-width / 2.0f, 0, height / 2.0f, 1.0f);
    glm::vec4 topRight(width / 2.0f, 0, height / 2.0f, 1.0f);
    glm::vec4 bottomLeft(-width / 2.0f, 0, -height / 2.0f, 1.0f);
    glm::vec4 bottomRight(width / 2.0f, 0, -height / 2.0f, 1.0f);

    // Then, transform them to the world space using the position and orientation
    glm::vec4 worldTopLeft = renderObject.orientation * topLeft + renderObject.position;
    RAYX_LOG << "World top left: " << worldTopLeft.x << ", " << worldTopLeft.y << ", " << worldTopLeft.z;
    glm::vec4 worldTopRight = renderObject.orientation * topRight + renderObject.position;
    RAYX_LOG << "World top right: " << worldTopRight.x << ", " << worldTopRight.y << ", " << worldTopRight.z;
    glm::vec4 worldBottomLeft = renderObject.orientation * bottomLeft + renderObject.position;
    RAYX_LOG << "World bottom left: " << worldBottomLeft.x << ", " << worldBottomLeft.y << ", " << worldBottomLeft.z;
    glm::vec4 worldBottomRight = renderObject.orientation * bottomRight + renderObject.position;
    RAYX_LOG << "World bottom right: " << worldBottomRight.x << ", " << worldBottomRight.y << ", " << worldBottomRight.z;

    // Create the random engine
    std::random_device rd;   // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Generate random colors
    glm::vec3 random_color_1(dis(gen), dis(gen), dis(gen));
    glm::vec3 random_color_2(dis(gen), dis(gen), dis(gen));
    glm::vec3 random_color_3(dis(gen), dis(gen), dis(gen));
    glm::vec3 random_color_4(dis(gen), dis(gen), dis(gen));

    // Assign random colors to vertices
    Vertex v1(worldBottomLeft, random_color_1);
    Vertex v2(worldTopLeft, random_color_2);
    Vertex v3(worldBottomRight, random_color_3);
    addTriangle(v1, v2, v3);
    Vertex v4(worldTopRight, random_color_4);
    addTriangle(v2, v3, v4);
}

// Function that adds vertex to scene if it doesn't exist, otherwise adds index of existing vertex
uint16_t Scene::addVertex(const Vertex v, Topography topography) {
    auto index = vertexExists(v);
    if (index.has_value()) {
        m_indices[topography].push_back(index.value());
        return index.value();
    } else {
        m_vertices.push_back(v);
        m_indices[topography].push_back(m_vertices.size() - 1);
        return m_vertices.size() - 1;
    }
}

// Function that returns index of vertex if it exists, otherwise returns none
std::optional<uint16_t> Scene::vertexExists(const Vertex v) const {
    for (int i = 0; i < m_vertices.size(); i++) {
        if (m_vertices[i].pos == v.pos && m_vertices[i].color == v.color) {
            return i;
        }
    }
    return std::nullopt;
}