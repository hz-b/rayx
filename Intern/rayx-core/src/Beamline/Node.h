#pragma once

#include <glm.hpp>
#include <memory>
#include <string>

#include "Core.h"

namespace rayx {

class Group;
class DesignSource;
class DesignElement;

namespace detail {
std::string createUniqueNodeName() {
    static size_t counter = 0;
    return std::format("<some_node_{}>", counter++);
}
}  // namespace detail

/**
 * @class BeamlineNode
 * @brief Base class for all nodes in the beamline hierarchy (scene/model graph).
 *
 * This abstract class defines the common interface for any node in a beamline.
 * Nodes can be groups, elements, or sources, each providing their own logic
 * for position/orientation and potential child relationships.
 */
class RAYX_API BeamlineNode {
    friend class Group;  // Needed so only group can access m_parent for BeamlineNodes

  public:
    BeamlineNode();
    BeamlineNode(std::string name);

    BeamlineNode(const BeamlineNode&) = delete;
    BeamlineNode& operator=(const BeamlineNode&) = delete;

    BeamlineNode(BeamlineNode&&) noexcept = default;
    BeamlineNode& operator=(BeamlineNode&&) noexcept = default;

    /**
     * Ensures proper cleanup of derived classes when deleting through a pointer
     * to BeamlineNode.
     */
    virtual ~BeamlineNode() = default;

    /**
     * @brief Creates a deep copy (clone) of this BeamlineNode.
     *
     * Derived classes must implement this to allow duplication of their internal state.
     * @return A unique_ptr to the newly cloned BeamlineNode.
     */
    virtual std::unique_ptr<BeamlineNode> clone() const = 0;

    /**
     * @brief Computes and returns the world-space position of this node.
     *
     * This method combines the local position of this node with the transformations
     * of all its parent nodes up to the root to compute the absolute position in world space.
     *
     * @return The world-space position as a glm::dvec3.
     */
    glm::dvec3 getWorldSpacePosition() const;

    /**
     * @brief Computes and returns the world-space rotation of this node.
     *
     * This method combines the local rotation of this node with the rotations
     * of all its parent nodes up to the root to compute the absolute rotation in world space.
     *
     * @return The world-space rotation as a Rotation object.
     */
    Rotation getWorldSpaceRotation() const;

    bool hasParent() const { return m_parent != nullptr; }

    const BeamlineNode* getParent() const { return m_parent; }
    BeamlineNode* getParent() { return m_parent; }

    const BeamlineNode* getRoot() const;
    BeamlineNode* getRoot();

    int getObjectId() const;

    virtual bool isGroup() const { return false; }    // Overridden in Group
    virtual bool isElement() const { return false; }  // Overridden in Element
    virtual bool isSource() const { return false; }   // Overridden in Source
    const Group* asGroup() const;
    Group* asGroup();
    const DesignSource* asSource() const;
    DesignSource* asSource();
    const DesignElement* asElement() const;
    DesignElement* asElement();

    std::string name = createUniqueNodeName();
    glm::dvec3 position;
    Rotation rotation = RotationAroundAxis{Degrees{0.0}, {0.0, 0.0, 1.0}};

  private:
    /**
     * @brief Pointer to this node's parent in the beamline hierarchy.
     *
     * May be null if this node is at the top-level (no parent).
     * Only Groups can be parents, the other two node types cannot have children.
     */
    BeamlineNode* m_parent = nullptr;
};

}  // namespace rayx
