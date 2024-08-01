#include "RenderObject.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "Debug/Instrumentor.h"
#include "Plotting.h"
#include "RayProcessing.h"
#include "Triangulation/GeometryUtils.h"
#include "Triangulation/Triangulate.h"

/**
 * Constructor sets up vertex and index buffers based on the input parameters.
 */
RenderObject::RenderObject(Device& device, glm::mat4 modelMatrix, const std::vector<VertexVariant> vertices, const std::vector<uint32_t>& indices,
                           Texture&& texture, std::shared_ptr<DescriptorSetLayout> setLayout, std::shared_ptr<DescriptorPool> descriptorPool)
    : m_Device(device),
      m_modelMatrix(modelMatrix),
      m_Texture(std::move(texture)),
      m_setLayout(setLayout),
      m_descriptorPool(descriptorPool),
      m_descrSet(VK_NULL_HANDLE) {
    assert(setLayout != nullptr && "Descriptor set layout is null");
    assert(descriptorPool != nullptr && "Descriptor pool is null");
    assert(vertices.size() > 0 && "Cannot create render object with no vertices");
    assert(indices.size() > 0 && "Cannot create render object with no indices");

    createVertexBuffers(vertices);
    createIndexBuffers(indices);
    createDescriptorSet();
}

RenderObject::RenderObject(RenderObject&& other) noexcept
    : m_Device(other.m_Device),
      m_modelMatrix(other.m_modelMatrix),
      m_Texture(std::move(other.m_Texture)),
      m_setLayout(other.m_setLayout),
      m_descriptorPool(other.m_descriptorPool),
      m_descrSet(other.m_descrSet),
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

        m_descrSet = other.m_descrSet;
        m_descriptorPool = other.m_descriptorPool;
        m_Texture = std::move(other.m_Texture);
        m_modelMatrix = other.m_modelMatrix;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;
        m_vertexBuffer = std::move(other.m_vertexBuffer);
        m_indexBuffer = std::move(other.m_indexBuffer);
        m_setLayout = other.m_setLayout;
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

void RenderObject::rebuild(const std::vector<VertexVariant> vertices, const std::vector<uint32_t>& indices) {
    vkDeviceWaitIdle(m_Device.device());
    createVertexBuffers(vertices);
    createIndexBuffers(indices);
}

void RenderObject::updateTexture(const std::filesystem::path& path) {
    m_Texture = Texture(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, {0, 0});
    m_Texture.updateFromPath(path);

    createDescriptorSet();
}

void RenderObject::updateTexture(const unsigned char* data, uint32_t width, uint32_t height) {
    if (data == nullptr) RAYX_EXIT << "Texture data is null";

    m_Texture = Texture(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, {width, height});
    m_Texture.updateFromData(data, width, height);

    createDescriptorSet();
}

void RenderObject::updateParams(const glm::mat4& modelMatrix, std::vector<VertexVariant>& vertices, std::vector<uint32_t>& indices) {
    m_modelMatrix = modelMatrix;
    rebuild(vertices, indices);
    createDescriptorSet();
}

void RenderObject::createVertexBuffers(const std::vector<VertexVariant>& vertices) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());

    // Get the size of the first vertex in the vector
    VkDeviceSize instanceSize = std::visit([](auto&& arg) { return sizeof(arg); }, vertices[0]);

    m_vertexBuffer = std::make_unique<Buffer>(m_Device, "rObjVertBuff", instanceSize, m_vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_vertexBuffer->map();

    // Write each vertex to the buffer
    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto& vertex = vertices[i];
        std::visit(
            [&](auto&& arg) {
                // You'll need to implement this write function for each vertex type
                m_vertexBuffer->writeToBuffer(&arg, sizeof(arg), sizeof(arg) * i);
            },
            vertex);
    }
}

void RenderObject::createIndexBuffers(const std::vector<uint32_t>& indices) {
    m_indexCount = static_cast<uint32_t>(indices.size());

    m_indexBuffer = std::make_unique<Buffer>(m_Device, "rObjIndexBuff", sizeof(indices[0]), m_indexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_indexBuffer->map();
    m_indexBuffer->writeToBuffer(indices.data());
}

void RenderObject::createDescriptorSet() {
    VkDescriptorImageInfo descrInfo = m_Texture.getDescriptorInfo();

    if (m_descrSet != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(m_Device.device(), m_descriptorPool->getDescriptorPool(), 1, &m_descrSet);
    }

    DescriptorWriter writer(*m_setLayout, *m_descriptorPool);
    writer.writeImage(0, &descrInfo);

    if (!writer.build(m_descrSet)) {
        RAYX_EXIT << "Failed to build descriptor set for texture";
    }
}