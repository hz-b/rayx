#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>

/**
 * @struct Vertex
 * @brief A structure representing a Vertex in 3D space.
 *
 * This structure contains position data for a vertex. It also provides methods
 * to generate Vulkan-specific descriptions for vertex input bindings and vertex input attributes.
 */
struct Vertex {
    /// @brief 4D vector representing the position of the vertex.
    glm::vec4 pos;

    Vertex() = default;
    Vertex(glm::vec3 pos) : pos(glm::vec4(pos, 1.0f)) {}
    Vertex(glm::vec4 pos) : pos(pos) {}

    virtual bool operator==(const Vertex& other) const = 0;

    std::vector<VkVertexInputBindingDescription> getBindingDescriptions() const {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() const {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, pos)});
        return attributeDescriptions;
    }
};

/**
 * @struct Vertex
 * @brief A structure representing a Vertex in 3D space with additional color information.
 *
 * This structure contains position and color data for a vertex. It also provides methods
 * to generate Vulkan-specific descriptions for vertex input bindings and vertex input attributes.
 */
struct TexVertex : public Vertex {
    /// @brief 2D vector representing the texture coordinates of the vertex.
    glm::vec2 uv;

    TexVertex() = default;
    TexVertex(glm::vec3 pos, glm::vec2 uv) : Vertex(pos), uv(uv) {}
    TexVertex(glm::vec4 pos, glm::vec2 uv) : Vertex(pos), uv(uv) {}

    /// @brief Equality operator for Vertex objects.
    bool operator==(const Vertex& other) const override {
        if (const TexVertex* v = dynamic_cast<const TexVertex*>(&other)) {
            return pos == v->pos && uv == v->uv;
        } else {
            return false;
        }
    }

    /**
     * @brief Generate Vulkan vertex input binding descriptions.
     *
     * Provides Vulkan-specific descriptions required for vertex input binding.
     *
     * @return A vector of VkVertexInputBindingDescription objects.
     */
    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(TexVertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    /**
     * @brief Generate Vulkan vertex input attribute descriptions.
     *
     * Provides Vulkan-specific descriptions required for vertex input attributes.
     *
     * @return A vector of VkVertexInputAttributeDescription objects.
     */
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(TexVertex, pos)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(TexVertex, uv)});
        return attributeDescriptions;
    }
};

/**
 * @struct Vertex
 * @brief A structure representing a Vertex in 3D space with additional color information.
 *
 * This structure contains position and color data for a vertex. It also provides methods
 * to generate Vulkan-specific descriptions for vertex input bindings and vertex input attributes.
 */
struct ColorVertex : public Vertex {
    /// @brief 4D vector representing the color of the vertex.
    glm::vec4 color;

    ColorVertex() = default;
    ColorVertex(glm::vec3 pos, glm::vec3 color) : Vertex(pos), color(glm::vec4(color, 1.0f)) {}
    ColorVertex(glm::vec3 pos, glm::vec4 color) : Vertex(pos), color(color) {}
    ColorVertex(glm::vec4 pos, glm::vec3 color) : Vertex(pos), color(glm::vec4(color, 1.0f)) {}
    ColorVertex(glm::vec4 pos, glm::vec4 color) : Vertex(pos), color(color) {}

    /// @brief Equality operator for Vertex objects.
    bool operator==(const Vertex& other) const override {
        if (const ColorVertex* v = dynamic_cast<const ColorVertex*>(&other)) {
            return pos == v->pos && color == v->color;
        } else {
            return false;
        }
    }

    /**
     * @brief Generate Vulkan vertex input binding descriptions.
     *
     * Provides Vulkan-specific descriptions required for vertex input binding.
     *
     * @return A vector of VkVertexInputBindingDescription objects.
     */
    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(ColorVertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    /**
     * @brief Generate Vulkan vertex input attribute descriptions.
     *
     * Provides Vulkan-specific descriptions required for vertex input attributes.
     *
     * @return A vector of VkVertexInputAttributeDescription objects.
     */
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ColorVertex, pos)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(ColorVertex, color)});
        return attributeDescriptions;
    }
};