#pragma once

#include <functional>
#include <memory>
#include <variant>
#include <vector>

#include <glm.hpp>

#include "Core.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"

namespace RAYX {

class Group;
using BeamlineNode = std::variant<DesignElement, DesignSource, Group>;
enum class NodeType { OpticalElement, LightSource, Group };

class RAYX_API Group {
  public:
    Group() = default;
    ~Group() = default;
    Group(Group&& other) noexcept;
    Group& operator=(Group&& other) noexcept;
    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    NodeType getNodeType() const;
    const BeamlineNode& getNode(size_t index) const;

    void traverse(const std::function<void(const BeamlineNode&)>& callback) const;

    void addChild(BeamlineNode&& child);

    // Returns the smallest possible MaterialTables which cover all materials of the elements in the group
    MaterialTables calcMinimalMaterialTables() const;
    // Compiles all elements and return vector of OpticalElements
    std::vector<OpticalElement> compileElements() const;
    std::vector<Ray> compileSources(int thread_count = 1) const;

    // New methods for retrieving elements, sources, and groups
    std::vector<const DesignElement*> getElements() const;
    std::vector<const DesignSource*> getSources() const;
    std::vector<const Group*> getGroups() const;
    size_t numElements() const;
    size_t numSources() const;

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
using Beamline = Group;  // Conceptually, a Beamline is a Group

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
