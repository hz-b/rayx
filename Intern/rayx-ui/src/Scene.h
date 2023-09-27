#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

#include "GraphicsCore/Buffer.h"
#include "GraphicsCore/Device.h"
#include "RenderObject.h"

// TODO: Refactor, so that Scene can be changed dynamically (currently, it is only set up once before rendering)
// Note: The OpticalElements only need to be (partially) updated, when the user interacts with them or when a new beamline is loaded.
//       All thhe rays need to be updated after every trace. Redesign might be necessary to support this.
class Scene {
  public:
    enum Topography { TRIA_TOPOGRAPHY = 0, LINE_TOPOGRAPHY = 1 };

  public:
    Scene(Device& device);
    Scene(Device& device, const std::vector<RenderObject>& rObjects, const std::vector<Line>& rays);
    ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    Scene& operator=(Scene&& other) {
        if (this != &other) {
            m_vertices = std::move(other.m_vertices);
            m_indices = std::move(other.m_indices);
            m_vertexBuffer = std::move(other.m_vertexBuffer);
            m_indexBuffers = std::move(other.m_indexBuffers);
        }
        return *this;
    }
    Scene(Scene&& other) : m_Device(other.m_Device) { *this = std::move(other); }

    // Buffers and drawing
    void updateBuffers();
    void bind(VkCommandBuffer commandBuffer, Topography topography) const;
    void draw(VkCommandBuffer commandBuffer, Topography topography) const;

    // Getters
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
};
