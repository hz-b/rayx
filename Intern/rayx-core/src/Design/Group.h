#pragma once

#include <functional>
#include <memory>
#include <variant>
#include <vector>

#include <glm.hpp>

#include "Design/DesignElement.h"
#include "Design/DesignSource.h"

namespace RAYX {

class Group;

using BeamlineNode = std::variant<DesignElement, DesignSource, Group>;

enum class NodeType { OpticalElement, LightSource, Group };

class Group {
  public:
    NodeType getNodeType() const;
    const BeamlineNode& getNode(size_t index) const;

    void traverse(const std::function<void(const BeamlineNode&)>& callback) const;
    // TODO: helper functions for num elemens/sources

    void addChild(BeamlineNode&& child);
    void addChild(const BeamlineNode& child);

    // New methods for retrieving elements, sources, and groups
    std::vector<DesignElement> getAllElements() const;
    std::vector<DesignSource> getAllSources() const;
    std::vector<Group> getAllGroups() const;

    // Getter & Setter
    const glm::dvec4& getPosition() const { return m_position; }
    const glm::dmat4& getOrientation() const { return m_orientation; }

    void setPosition(const glm::dvec4& pos) { m_position = pos; }
    void setOrientation(const glm::dmat4& orientation) { m_orientation = orientation; }

  private:
    glm::dvec4 m_position = glm::dvec4(0, 0, 0, 1);
    glm::dmat4 m_orientation = glm::dmat4(1);

    std::vector<BeamlineNode> children;  // Children of the node
};

// Utility function to determine node type
inline NodeType getNodeType(const BeamlineNode& node) {
    return std::visit(
        [](auto&& element) -> NodeType {
            using T = std::decay_t<decltype(element)>;
            if constexpr (std::is_same_v<T, DesignElement>) {
                return NodeType::OpticalElement;
            } else if constexpr (std::is_same_v<T, DesignSource>) {
                return NodeType::LightSource;
            } else if constexpr (std::is_same_v<T, Group>) {
                return NodeType::Group;
            }
        },
        node);
}
}  // namespace RAYX
