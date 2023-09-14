#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "Vertex.h"

#define GRIDSIZE 10

struct Triangle {
    Vertex v1;
    Vertex v2;
    Vertex v3;
};
struct Line {
    Vertex v1;
    Vertex v2;
};

class RenderObject {
  public:
    RenderObject(glm::mat4 modelMatrix) : m_modelMatrix(modelMatrix) {}
    void addTriangle(Triangle t) {
        m_vertices.push_back(t.v1);
        m_vertices.push_back(t.v2);
        m_vertices.push_back(t.v3);
    }

    std::vector<Vertex> getWorldVertices() const {
        std::vector<Vertex> worldVertices;
        for (Vertex v : m_vertices) {
            Vertex worldVertex;
            worldVertex.pos = m_modelMatrix * v.pos;
            worldVertex.color = v.color;
            worldVertices.push_back(worldVertex);
        }
        return worldVertices;
    }

  private:
    std::vector<Vertex> m_vertices;  ///< In model coordinates
    glm::mat4 m_modelMatrix;         ///< To transform the object from model to world coordinates
};