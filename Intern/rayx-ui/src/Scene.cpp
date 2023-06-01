#include "Scene.h"

Scene::Scene() {
    addTriangle({{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}});
    addTriangle({{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}, {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}});
    addLine({{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}});
    addLine({{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}});
}

void Scene::addTriangle(const Vertex v1, const Vertex v2, const Vertex v3) {
    addVertex(v1, TRI_TOPOGRAPHY);
    addVertex(v2, TRI_TOPOGRAPHY);
    addVertex(v3, TRI_TOPOGRAPHY);
}

void Scene::addLine(const Vertex v1, const Vertex v2) {
    addVertex(v1, LIN_TOPOGRAPHY);
    addVertex(v2, LIN_TOPOGRAPHY);
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