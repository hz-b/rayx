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

/**
 * @class RenderObject
 * @brief Class for handling the rendering of objects in Vulkan.
 *
 * This class manages Vulkan-specific vertex and index buffers for rendering a graphical object.
 * It also handles the binding and drawing operations for these objects.
 */
class RenderObject {
  public:
    /**
     * @brief Constructor that initializes the rendering object.
     * @param device Reference to a Device object.
     * @param modelMatrix Transformation matrix to go from model to world coordinates.
     * @param vertices Vector of Vertex objects.
     * @param indices Vector of index values.
     */
    RenderObject(Device& device, glm::mat4 modelMatrix, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

    /**
     * @brief Binds the object's vertex and index buffers to a Vulkan command buffer.
     * @param commandBuffer Vulkan command buffer.
     */
    void bind(VkCommandBuffer commandBuffer) const;

    /**
     * @brief Issues a draw call for the object using the Vulkan command buffer.
     * @param commandBuffer Vulkan command buffer.
     */
    void draw(VkCommandBuffer commandBuffer) const;

    glm::mat4 getModelMatrix() const { return m_modelMatrix; }
    glm::vec3 getTranslationVecor() const { return m_translationVector; }

  private:
    void createVertexBuffers(const std::vector<Vertex>& vertices);
    void createIndexBuffers(const std::vector<uint32_t>& indices);

    Device& m_Device;

    uint32_t m_vertexCount;
    uint32_t m_indexCount;

    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;

    glm::mat4 m_modelMatrix;  ///< Matrix for transforming the object from model to world coordinates

    // 3D Space specfic
    glm::vec3 m_translationVector;
    glm::vec3 m_skewVector;
    glm::vec3 m_rotationVector;  // Radians
    glm::vec3 m_scaleVector;
    glm::vec4 m_perspective;
};