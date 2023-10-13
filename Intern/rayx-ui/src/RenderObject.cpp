#include "RenderObject.h"

/**
 * Constructor sets up vertex and index buffers based on the input parameters.
 */
RenderObject::RenderObject(Device& device, glm::mat4 modelMatrix, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
    : m_Device(device), m_modelMatrix(modelMatrix) {
    createVertexBuffers(vertices);
    createIndexBuffers(indices);
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

    m_indexBuffer = std::make_unique<Buffer>(m_Device,"rObjIndexBuff", sizeof(indices[0]), m_indexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_indexBuffer->map();
    m_indexBuffer->writeToBuffer(indices.data());
}