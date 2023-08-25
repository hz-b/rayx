#include "Scene.h"

#include "Beamline/OpticalElement.h"
#include "Debug/Debug.h"

Scene::Scene(Device& device) : m_Device(device) {}

void Scene::setup(const RAYX::RenderObjectVec& renderObjects, const RAYX::BundleHistory& bundleHistory) {
    for (const auto& renderObject : renderObjects) {
        fromRenderObject(renderObject);
    }

    // Event types:
    // const double ETYPE_FLY_OFF = 0;       --> World coordinates
    // const double ETYPE_JUST_HIT_ELEM = 1; --> Element coordinates
    // const double ETYPE_ABSORBED = 3;      --> Element coordinates
    // Rest are error codes
    glm::vec3 yellow = {1.0f, 1.0f, 0.0f};
    glm::vec3 blue = {0.0f, 0.0f, 1.0f};
    for (const auto& rayHist : bundleHistory) {
        glm::vec3 rayLastPos = {0.0f, 0.0f, 0.0f};
        for (const auto& event : rayHist) {
            if (event.m_eventType == ETYPE_JUST_HIT_ELEM) {
                // Events where rays hit objects are in element coordinates
                // We need to convert them to world coordinates
                glm::vec4 lastElementPos = renderObjects[(size_t)event.m_lastElement].position;
                glm::mat4 lastElementOri = renderObjects[(size_t)event.m_lastElement].orientation;
                glm::dmat4 outTrans = RAYX::calcTransformationMatrices(lastElementPos, lastElementOri, false);
                glm::vec4 worldPos = outTrans * glm::vec4(event.m_position, 1.0f);

                Vertex origin = {{rayLastPos.x, rayLastPos.y, rayLastPos.z}, yellow};
                Vertex point = {{worldPos.x, worldPos.y, worldPos.z}, yellow};
                addLine(origin, point);

                rayLastPos = point.pos;
            } else if (event.m_eventType == ETYPE_FLY_OFF) {
                // The origin here is the position of the event
                // And the point towards the direction of the ray
                // This ray flies off into infinity, so we need to calculate a sensible
                glm::vec3 eventPos = event.m_position;
                glm::vec3 eventDir = event.m_direction;
                glm::vec3 pointPos = eventPos + eventDir * 1000.0f;

                Vertex origin = {{eventPos.x, eventPos.y, eventPos.z}, blue};
                Vertex point = {{pointPos.x, pointPos.y, pointPos.z}, blue};
                addLine(origin, point);
            }
        }
    }

    updateBuffers();
}

void Scene::addTriangle(const Vertex v1, const Vertex v2, const Vertex v3) {
    addVertex(v1, TRIA_TOPOGRAPHY);
    addVertex(v2, TRIA_TOPOGRAPHY);
    addVertex(v3, TRIA_TOPOGRAPHY);
}

void Scene::addLine(const Vertex v1, const Vertex v2) {
    addVertex(v1, LINE_TOPOGRAPHY);
    addVertex(v2, LINE_TOPOGRAPHY);
}

void Scene::fromRenderObject(const RAYX::RenderObject& renderObject) {
    if (renderObject.cutout.m_type != CTYPE_RECT) {
        RAYX_ERR << "Unsupported cutout type!";
    }

    auto rect = deserializeRect(renderObject.cutout);
    auto width = rect.m_size_x1;
    auto height = rect.m_size_x2;

    // Calculate two triangle from the position, orientation and size of the render object
    // Assume the rectangle is in the XZ plane, and its center is at position.

    // First, calculate the corners of the rectangle in the model's local space
    glm::vec4 topLeft(-width / 2.0f, 0, height / 2.0f, 1.0f);
    glm::vec4 topRight(width / 2.0f, 0, height / 2.0f, 1.0f);
    glm::vec4 bottomLeft(-width / 2.0f, 0, -height / 2.0f, 1.0f);
    glm::vec4 bottomRight(width / 2.0f, 0, -height / 2.0f, 1.0f);

    // Then, transform them to the world space using the position and orientation
    glm::vec4 worldTopLeft = renderObject.orientation * topLeft + renderObject.position;
    RAYX_LOG << "World top left: " << worldTopLeft.x << ", " << worldTopLeft.y << ", " << worldTopLeft.z;
    glm::vec4 worldTopRight = renderObject.orientation * topRight + renderObject.position;
    RAYX_LOG << "World top right: " << worldTopRight.x << ", " << worldTopRight.y << ", " << worldTopRight.z;
    glm::vec4 worldBottomLeft = renderObject.orientation * bottomLeft + renderObject.position;
    RAYX_LOG << "World bottom left: " << worldBottomLeft.x << ", " << worldBottomLeft.y << ", " << worldBottomLeft.z;
    glm::vec4 worldBottomRight = renderObject.orientation * bottomRight + renderObject.position;
    RAYX_LOG << "World bottom right: " << worldBottomRight.x << ", " << worldBottomRight.y << ", " << worldBottomRight.z;

    glm::vec4 purple = {0.5f, 0.0f, 0.5f, 1.0f};
    glm::vec4 pink = {1.0f, 0.0f, 1.0f, 1.0f};
    glm::vec4 cyan = {0.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 red = {1.0f, 0.0f, 0.0f, 1.0f};
    Vertex v1(worldBottomLeft, purple);
    Vertex v2(worldTopLeft, pink);
    Vertex v3(worldBottomRight, red);
    addTriangle(v1, v2, v3);
    Vertex v4(worldTopRight, cyan);
    addTriangle(v2, v3, v4);
}

void Scene::draw(VkCommandBuffer commandBuffer, Topography topography) const {
    if (m_indexBuffers[topography] != nullptr) {
        vkCmdDrawIndexed(commandBuffer, (uint32_t)m_indices[topography].size(), 1, 0, 0, 0);
    } else {
        vkCmdDraw(commandBuffer, (uint32_t)m_vertices.size(), 1, 0, 0);
    }
}

void Scene::bind(VkCommandBuffer commandBuffer, Topography topography) const {
    VkBuffer buffers[] = {m_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (m_indexBuffers[topography] != nullptr) {
        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffers[topography]->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}

// Function that adds vertex to scene if it doesn't exist, otherwise adds index of existing vertex
uint32_t Scene::addVertex(const Vertex v, Topography topography) {
    auto index = vertexExists(v);
    if (index.has_value()) {
        m_indices[topography].push_back(index.value());
        return index.value();
    } else {
        m_vertices.push_back(v);
        m_indices[topography].push_back((uint32_t)m_vertices.size() - 1);
        return (uint32_t)m_vertices.size() - 1;
    }
}

// Function that returns index of vertex if it exists, otherwise returns none
std::optional<uint32_t> Scene::vertexExists(const Vertex v) const {
    for (int i = 0; i < m_vertices.size(); i++) {
        if (m_vertices[i].pos == v.pos && m_vertices[i].color == v.color) {
            return i;
        }
    }
    return std::nullopt;
}

void Scene::updateBuffers() {
    updateVertexBuffer();
    updateIndexBuffers();
}

void Scene::updateVertexBuffer() {
    // update the vertex buffer
    m_vertexBuffer = std::make_unique<Buffer>(m_Device, sizeof(Vertex), (uint32_t)m_vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_vertexBuffer->map();
    m_vertexBuffer->writeToBuffer(m_vertices.data(), m_vertices.size() * sizeof(Vertex));
    m_vertexBuffer->flush();
}

void Scene::updateIndexBuffers() {
    // update the index buffers
    m_indexBuffers[0] = std::make_unique<Buffer>(m_Device, sizeof(uint32_t), (uint32_t)m_indices[0].size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_indexBuffers[0]->map();
    m_indexBuffers[0]->writeToBuffer(m_indices[0].data(), m_indices[0].size() * sizeof(uint32_t));
    m_indexBuffers[0]->flush();

    m_indexBuffers[1] = std::make_unique<Buffer>(m_Device, sizeof(uint32_t), (uint32_t)m_indices[1].size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_indexBuffers[1]->map();
    m_indexBuffers[1]->writeToBuffer(m_indices[1].data(), m_indices[1].size() * sizeof(uint32_t));
    m_indexBuffers[1]->flush();
}