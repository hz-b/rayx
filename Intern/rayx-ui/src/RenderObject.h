#pragma once

#include <glm.h>

#include <vector>

#include "GraphicsCore/Buffer.h"
#include "GraphicsCore/Descriptors.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/Texture.h"
#include "Tracer/Tracer.h"
#include "Vertex.h"

struct Triangle {
    TextureVertex v1;
    TextureVertex v2;
    TextureVertex v3;
};
struct Line {
    ColorVertex v1;
    ColorVertex v2;
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
     * @param name Render Object name, usually read from Beamline
     * @param device Reference to a Device object.
     * @param modelMatrix Transformation matrix to go from model to world coordinates.
     * @param vertices Vector of Vertex objects.
     * @param indices Vector of index values.
     */
    RenderObject(Device& device, glm::mat4 modelMatrix, const std::vector<VertexVariant> vertices, const std::vector<uint32_t>& indices,
                 Texture&& texture, std::shared_ptr<DescriptorSetLayout> setLayout, std::shared_ptr<DescriptorPool> descriptorPool);
    RenderObject(const RenderObject&) = delete;
    RenderObject& operator=(const RenderObject&) = delete;
    RenderObject(RenderObject&& other) noexcept;
    RenderObject& operator=(RenderObject&& other) noexcept;

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

    void rebuild(const std::vector<VertexVariant> vertices, const std::vector<uint32_t>& indices);

    void updateTexture(const std::filesystem::path& path);
    void updateTexture(const unsigned char* data, uint32_t width, uint32_t height);

    void updateParams(const glm::mat4& modelMatrix, std::vector<VertexVariant>& vertices, std::vector<uint32_t>& indices);

    glm::mat4 getModelMatrix() const { return m_modelMatrix; }
    glm::vec3 getTranslationVecor() const { return glm::vec3(m_modelMatrix[3][0], m_modelMatrix[3][1], m_modelMatrix[3][2]); }
    VkDescriptorSet getDescriptorSet() const { return m_descrSet; }

  private:
    void createVertexBuffers(const std::vector<VertexVariant>& vertices);
    void createIndexBuffers(const std::vector<uint32_t>& indices);
    void createDescriptorSet();

    Device& m_Device;
    glm::mat4 m_modelMatrix;  ///< Matrix for transforming the object from model to world coordinates
    Texture m_Texture;

    std::shared_ptr<DescriptorSetLayout> m_setLayout;
    std::shared_ptr<DescriptorPool> m_descriptorPool;
    VkDescriptorSet m_descrSet;

    uint32_t m_vertexCount;
    uint32_t m_indexCount;

    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;
};