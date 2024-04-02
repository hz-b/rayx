#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>
#include <variant>

/**
 * @struct Vertex
 * @brief A structure representing a Vertex in 3D space with additional color information.
 *
 * This structure contains position and color data for a vertex. It also provides methods
 * to generate Vulkan-specific descriptions for vertex input bindings and vertex input attributes.
 */
struct TextureVertex {
    /// @brief 4D vector representing the position of the vertex.
    glm::vec4 pos;

    /// @brief 2D vector representing the texture coordinates of the vertex.
    glm::vec2 uv;

    /// @brief Equality operator for TexVertex objects.
    bool operator==(const TextureVertex& other) const { return pos == other.pos && uv == other.uv; }

    void print() const {
        RAYX_LOG << "Vertex: Position " << pos.x << ", " << pos.y << ", " << pos.z << ", " << pos.w << " | UV " << uv.x << ", " << uv.y;
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
        bindingDescriptions[0].stride = sizeof(TextureVertex);
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
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(TextureVertex, pos)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(TextureVertex, uv)});
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
struct ColorVertex {
    /// @brief 4D vector representing the position of the vertex.
    glm::vec4 pos;

    /// @brief 4D vector representing the color of the vertex.
    glm::vec4 color;

    /// @brief Equality operator for ColorVertex objects.
    bool operator==(const ColorVertex& other) const { return pos == other.pos && color == other.color; }

    void print() const {
        RAYX_LOG << "Vertex: Position " << pos.x << ", " << pos.y << ", " << pos.z << ", " << pos.w << " | Color " << color.x << ", " << color.y
                 << ", " << color.z << ", " << color.w;
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
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(ColorVertex, pos)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(ColorVertex, color)});
        return attributeDescriptions;
    }
};

using VertexVariant = std::variant<TextureVertex, ColorVertex>;