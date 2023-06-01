#include "Scene.h"

Scene::Scene() {
    addTriangle({{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}});
    addTriangle({{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}, {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}});
    addLine({{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}});
    addLine({{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}});
}

void Scene::addTriangle(Vertex v1, Vertex v2, Vertex v3) {
    addVertex(v1);
    addVertex(v2);
    addVertex(v3);
}
void Scene::addLine(Vertex v1, Vertex v2) {
    auto index = addVertex(v1);
    addVertex(v2);
    m_indices.push_back(index);
}

// Function that adds vertex to scene if it doesn't exist, otherwise adds index of existing vertex
uint16_t Scene::addVertex(Vertex v) {
    auto index = vertexExists(v);
    if (index.has_value()) {
        m_indices.push_back(index.value());
        return index.value();
    } else {
        m_vertices.push_back(v);
        m_indices.push_back(m_vertices.size() - 1);
        return m_vertices.size() - 1;
    }
}

// Function that returns index of vertex if it exists, otherwise returns none
std::optional<uint16_t> Scene::vertexExists(Vertex v) const {
    for (int i = 0; i < m_vertices.size(); i++) {
        if (m_vertices[i].pos == v.pos && m_vertices[i].color == v.color) {
            return i;
        }
    }
    return std::nullopt;
}