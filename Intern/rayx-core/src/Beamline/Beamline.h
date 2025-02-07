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
using BeamlineNode = std::variant<std::unique_ptr<DesignElement>, std::unique_ptr<DesignSource>, std::unique_ptr<Group>>;
enum class NodeType { OpticalElement, LightSource, Group };

class RAYX_API Group {
  public:
    Group() = default;
    ~Group() = default;
    Group(Group&& other) noexcept;
    Group& operator=(Group&& other) noexcept;
    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    // Clone returns a deep copy of the group and its children.
    Group clone() const;

    template <typename Callback>
    void traverse(Callback&& callback) const;

    // Iterators for non-const access
    auto begin() { return m_children.begin(); }
    auto end() { return m_children.end(); }

    // Iterators for const access
    auto begin() const { return m_children.cbegin(); }
    auto end() const { return m_children.cend(); }
    auto cbegin() const { return m_children.cbegin(); }
    auto cend() const { return m_children.cend(); }

    NodeType getNodeType() const;
    std::vector<BeamlineNode>& getChildren() { return m_children; }

    // Add a child (by move).
    void addChild(BeamlineNode&& child);

    // Getters returning raw pointers. This follows more the old way of handling beamlines and is to be used with care.
    std::vector<DesignElement*> getElements() const;
    std::vector<DesignSource*> getSources() const;
    std::vector<Group*> getGroups() const;

    // Helper
    size_t numElements() const;
    size_t numSources() const;
    MaterialTables calcMinimalMaterialTables() const;
    std::vector<OpticalElement> compileElements() const;
    std::vector<Ray> compileSources(int thread_count = 1) const;
    static void accumulateLightSourcesWorldPositions(const Group& group, const glm::dvec4& parentPos, const glm::dmat4& parentOri,
                                                     std::vector<glm::dvec4>& positions);

    // Getters & setters for transforms.
    const glm::dvec4& getPosition() const { return m_position; }
    const glm::dmat4& getOrientation() const { return m_orientation; }
    void setPosition(const glm::dvec4& pos) { m_position = pos; }
    void setOrientation(const glm::dmat4& orientation) { m_orientation = orientation; }

  private:
    glm::dvec4 m_position = glm::dvec4(0, 0, 0, 1);
    glm::dmat4 m_orientation = glm::dmat4(1);
    std::vector<BeamlineNode> m_children;
};
using Beamline = Group;  // Conceptually, a Beamline is a Group

// Utility function to determine node type.
inline NodeType getNodeType(const BeamlineNode& node) {
    return std::visit(
        [](auto&& element) -> NodeType {
            using T = std::decay_t<decltype(element)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<DesignElement>>) {
                return NodeType::OpticalElement;
            } else if constexpr (std::is_same_v<T, std::unique_ptr<DesignSource>>) {
                return NodeType::LightSource;
            } else if constexpr (std::is_same_v<T, std::unique_ptr<Group>>) {
                return NodeType::Group;
            }
        },
        node);
}

}  // namespace RAYX
