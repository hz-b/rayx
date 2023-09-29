#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "GraphicsCore/Buffer.h"
#include "GraphicsCore/Device.h"
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
    RenderObject(Device& device, glm::mat4 modelMatrix, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
        : m_Device(device), m_modelMatrix(modelMatrix) {
        createVertexBuffers(vertices);
        createIndexBuffers(indices);
    }

    glm::mat4 getModelMatrix() const { return m_modelMatrix; }
    void bind(VkCommandBuffer commandBuffer) const;
    void draw(VkCommandBuffer commandBuffer) const;

  private:
    void createVertexBuffers(const std::vector<Vertex>& vertices);
    void createIndexBuffers(const std::vector<uint32_t>& indices);

    Device& m_Device;

    uint32_t m_vertexCount;
    uint32_t m_indexCount;

    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;

    glm::mat4 m_modelMatrix;  ///< To transform the object from model to world coordinates
};