#include "RenderObject.h"

#include <glm/gtx/matrix_decompose.hpp>
/**
 * Constructor sets up vertex and index buffers based on the input parameters.
 */
RenderObject::RenderObject(std::string name, Device& device, glm::mat4 modelMatrix, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
    : m_name(name), m_Device(device), m_modelMatrix(modelMatrix) {
    createVertexBuffers(vertices);
    createIndexBuffers(indices);
}

RenderObject::RenderObject(RenderObject&& other) noexcept
    : m_name(std::move(other.m_name)),
      m_Device(other.m_Device),
      m_descrSetTexture(std::move(other.m_descrSetTexture)),
      m_modelMatrix(other.m_modelMatrix),
      m_vertexCount(other.m_vertexCount),
      m_indexCount(other.m_indexCount),
      m_vertexBuffer(std::move(other.m_vertexBuffer)),
      m_indexBuffer(std::move(other.m_indexBuffer)) {}

RenderObject& RenderObject::operator=(RenderObject&& other) noexcept {
    if (this != &other) {
        if (m_Device.device() != other.m_Device.device()) {
            // If this warning is thrown you hopefully know what you're doing. I would not recommend using multiple devices.
            // If you don't know what's going on, you need to check why the devices are different.
            // If unhandled this will result in bugs and/or crashes.
            RAYX_WARN << "Cannot transfer ownership of resources between logical devices. Failing gracefully.";
            return *this;
        }

        m_name = std::move(other.m_name);
        m_descrSetTexture = std::move(other.m_descrSetTexture);
    }
    return *this;
}

void RenderObject::bind(VkCommandBuffer commandBuffer) const {
    VkBuffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void RenderObject::draw(VkCommandBuffer commandBuffer) const {
    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);  //
}

void RenderObject::createVertexBuffers(const std::vector<Vertex>& vertices) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());

    m_vertexBuffer = std::make_unique<Buffer>(m_Device, "rObjVertBuff", sizeof(vertices[0]), m_vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_vertexBuffer->map();
    m_vertexBuffer->writeToBuffer(vertices.data());
}

void RenderObject::createIndexBuffers(const std::vector<uint32_t>& indices) {
    m_indexCount = static_cast<uint32_t>(indices.size());

    m_indexBuffer = std::make_unique<Buffer>(m_Device, "rObjIndexBuff", sizeof(indices[0]), m_indexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_indexBuffer->map();
    m_indexBuffer->writeToBuffer(indices.data());
}