#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

class Scene {
  public:
    enum Topography { TRIA_TOPOGRAPHY = 0, LINE_TOPOGRAPHY = 1 };

  public:
    Scene();
    ~Scene() = default;

    void addTriangle(const Vertex v1, const Vertex v2, const Vertex v3);
    void addLine(const Vertex v1, const Vertex v2);

    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    const std::vector<uint16_t>& getIndices(Topography topography) const { return m_indices[topography]; }

  private:
    std::vector<Vertex> m_vertices;
    // Array of vectors of indices for each topography
    // 0 = triangles
    // 1 = lines
    std::array<std::vector<uint16_t>, 2> m_indices;

    uint16_t addVertex(const Vertex v, Topography topography);
    std::optional<uint16_t> vertexExists(const Vertex v) const;
};
