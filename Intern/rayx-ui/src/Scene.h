#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>
#include <vector>

#include "Buffer.h"
#include "Data/Importer.h"
#include "Device.h"
#include "RenderObject.h"
#include "Tracer/Tracer.h"

// TODO: Refactor, so that Scene can be changed dynamically (currently, it is only set up once before rendering)
class Scene {
  public:
    enum Topography { TRIA_TOPOGRAPHY = 0, LINE_TOPOGRAPHY = 1 };

  public:
    Scene(Device& device);
    ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    // Base functions
    void update(const std::vector<RenderObject>& renderObjects, const RAYX::BundleHistory& bundleHistory);
    // void addTriangle(const Vertex v1, const Vertex v2, const Vertex v3);
    void addLine(const Vertex v1, const Vertex v2);

    // Buffers and drawing
    void updateBuffers();
    void bind(VkCommandBuffer commandBuffer, Topography topography) const;
    void draw(VkCommandBuffer commandBuffer, Topography topography) const;

    // Getters
    // const std::vector<Vertex>& getVertices() const { return m_vertices; }
    // const std::vector<uint32_t>& getIndices(Topography topography) const { return m_indices[topography]; }
    // const size_t getIndexCount(Topography topography) const { return m_indices[topography].size(); }
    VkBuffer getVertexBuffer() const { return m_vertexBuffer->getBuffer(); }
    VkBuffer getIndexBuffer(Topography topography) const { return m_indexBuffers[topography]->getBuffer(); }

  private:
    Device& m_Device;

    std::vector<Vertex> m_vertices;
    std::array<std::vector<uint32_t>, 2> m_indices;  // 0 = triangles, 1 = lines

    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::array<std::unique_ptr<Buffer>, 2> m_indexBuffers = {nullptr, nullptr};  // 0 = triangles, 1 = lines

    uint32_t addVertex(const Vertex v, Topography topography);
    std::optional<uint32_t> vertexExists(const Vertex v) const;

    void updateVertexBuffer();
    void updateIndexBuffers();

    // Colors
    const glm::vec4 m_yellow = {1.0f, 1.0f, 0.0f, 1.0f};
    const glm::vec4 m_blue = {0.0f, 0.0f, 1.0f, 1.0f};
    const glm::vec4 m_red = {1.0f, 0.0f, 0.0f, 1.0f};
};
