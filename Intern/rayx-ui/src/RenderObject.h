#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Beamline/OpticalElement.h"
#include "GraphicsCore/Buffer.h"
#include "GraphicsCore/Descriptors.h"
#include "GraphicsCore/Device.h"
#include "GraphicsCore/Texture.h"
#include "Vertex.h"

struct Triangle {
    Vertex v1;
    Vertex v2;
    Vertex v3;
};
struct Line {
    Vertex v1;
    Vertex v2;
};
struct DescriptorSetTexture {
    VkDescriptorSet descrSet;
    Texture tex;
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
    RenderObject(std::string name, Device& device, glm::mat4 modelMatrix, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
                 std::shared_ptr<DescriptorSetLayout> setLayout);
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

    void updateTexture(const std::filesystem::path& path, const DescriptorPool& descriptorPool);

    glm::mat4 getModelMatrix() const { return m_modelMatrix; }
    glm::vec3 getTranslationVecor() const { return glm::vec3(m_modelMatrix[3][0], m_modelMatrix[3][1], m_modelMatrix[3][2]); }
    std::string getName() const { return m_name; }

    bool getDescriptorSet(VkDescriptorSet& outDescriptorSet) const;

    static std::vector<RenderObject> buildRObjectsFromElements(Device& device, const std::vector<RAYX::OpticalElement>& elements,
                                                               std::shared_ptr<DescriptorSetLayout> setLayout);

  private:
    void createVertexBuffers(const std::vector<Vertex>& vertices);
    void createIndexBuffers(const std::vector<uint32_t>& indices);

    std::string m_name;
    Device& m_Device;
    std::shared_ptr<DescriptorSetLayout> m_setLayout;
    std::optional<DescriptorSetTexture> m_descrSetTexture;

    uint32_t m_vertexCount;
    uint32_t m_indexCount;

    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;

    glm::mat4 m_modelMatrix;  ///< Matrix for transforming the object from model to world coordinates
};