#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>

/**
 * @struct Vertex
 * @brief A structure representing a Vertex in 3D space with additional color information.
 *
 * This structure contains position and color data for a vertex. It also provides methods
 * to generate Vulkan-specific descriptions for vertex input bindings and vertex input attributes.
 */
struct Vertex {
    /// @brief 4D vector representing the position of the vertex.
    glm::vec4 pos;
    /// @brief 4D vector representing the color of the vertex.
    glm::vec4 color;
    /// @brief 2D vector representing the texture coordinates of the vertex.
    glm::vec2 texCoord;

    /// @brief Equality operator for Vertex objects.
    bool operator==(const Vertex& other) const { return pos == other.pos && color == other.color; }

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
        bindingDescriptions[0].stride = sizeof(Vertex);
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
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord)});
        return attributeDescriptions;
    }
};