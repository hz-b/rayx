#include "RenderObject.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "Triangulation/Triangulate.h"

std::vector<RenderObject> RenderObject::buildRObjectsFromElements(Device& device, const std::vector<RAYX::OpticalElement>& elements) {
    std::vector<RenderObject> rObjects;
    std::shared_ptr<DescriptorSetLayout> setLayout = std::move(
        DescriptorSetLayout::Builder(device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT).build());

    for (const RAYX::OpticalElement& element : elements) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        triangulateObject(element, vertices, indices);

        glm::mat4 modelMatrix = element.m_element.m_outTrans;

        rObjects.emplace_back(element.m_name, device, modelMatrix, vertices, indices, setLayout);
    }

    std::cout << "Triangulation complete" << std::endl;
    return rObjects;
}

/**
 * Constructor sets up vertex and index buffers based on the input parameters.
 */
RenderObject::RenderObject(std::string name, Device& device, glm::mat4 modelMatrix, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
                           std::shared_ptr<DescriptorSetLayout> setLayout)
    : m_name(name), m_Device(device), m_modelMatrix(modelMatrix), m_setLayout(std::move(setLayout)) {
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
      m_indexBuffer(std::move(other.m_indexBuffer)),
      m_setLayout(std::move(other.m_setLayout)) {}

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
        m_modelMatrix = other.m_modelMatrix;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;
        m_vertexBuffer = std::move(other.m_vertexBuffer);
        m_indexBuffer = std::move(other.m_indexBuffer);
        m_setLayout = std::move(other.m_setLayout);
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

void RenderObject::updateTexture(const std::filesystem::path& path, const DescriptorPool& descriptorPool) {
    if (m_setLayout == nullptr) {
        RAYX_ERR << "Render objects descriptor set layout not initialized";
        return;
    }

    Texture tex(m_Device, path);

    std::shared_ptr<VkDescriptorImageInfo> descrInfo = tex.descriptorInfo();

    DescriptorWriter writer(*m_setLayout, descriptorPool);
    writer.writeImage(0, descrInfo.get());

    VkDescriptorSet descrSet;
    if (!writer.build(descrSet)) {
        RAYX_ERR << "Failed to build descriptor set for texture";
    }

    m_descrSetTexture = DescriptorSetTexture{descrSet, std::move(tex)};
}

bool RenderObject::getDescriptorSet(VkDescriptorSet& outDescriptorSet) const {
    if (m_descrSetTexture) {
        outDescriptorSet = m_descrSetTexture->descrSet;
        return true;
    } else {
        return false;
    }
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