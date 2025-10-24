#pragma once

#include <glm.hpp>
#include <memory>
#include <string>

#include "Core.h"

namespace rayx {

class Group;
class DesignSource;
class DesignElement;

/**
 * @class BeamlineNode
 * @brief Base class for all nodes in the beamline hierarchy (scene/model graph).
 *
 * This abstract class defines the common interface for any node in a beamline.
 * Nodes can be groups, elements, or sources, each providing their own logic
 * for position/orientation and potential child relationships.
 */
class RAYX_API BeamlineNode {
    friend class Group;  // Needed so group can access m_parent for BeamlineNodes
  public:
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

    virtual bool isGroup() const { return false; }    // Overridden in Group
    virtual bool isElement() const { return false; }  // Overridden in Element
    virtual bool isSource() const { return false; }   // Overridden in Source

    /**
     * @brief Gets the local position of this node.
     *
     * Each node may have its own transform relative to its parent.
     * @return A 4D vector (dvec4) representing the local position.
     */
    virtual glm::dvec4 getPosition() const = 0;

    /**
     * @brief Gets the local orientation (rotation/transform) of this node.
     *
     * Each node may have its own transform relative to its parent.
     * @return A 4x4 matrix (dmat4) representing the local orientation.
     */
    virtual glm::dmat4 getOrientation() const = 0;

    /**
     * @brief Computes the absolute/world position of this node.
     *
     * Recursively applies parent transformations until the root of the hierarchy
     * is reached. If there is no parent, the local position is already in world space.
     * @return A 4D vector (dvec4) representing the position in world coordinates.
     */
    glm::dvec4 getWorldPosition() const;

    /**
     * @brief Computes the absolute/world orientation of this node.
     *
     * Recursively multiplies the parent's world orientation by the node's local orientation.
     * If there is no parent, the local orientation is already in world space.
     * @return A 4x4 matrix (dmat4) representing the orientation in world coordinates.
     */
    glm::dmat4 getWorldOrientation() const;

    virtual std::string getName() const    = 0;
    virtual void setName(std::string name) = 0;

    bool hasParent() const { return m_parent != nullptr; }

    const BeamlineNode* getParent() const { return m_parent; }
    BeamlineNode* getParent() { return m_parent; }

    const BeamlineNode* getRoot() const;
    BeamlineNode* getRoot();

    int getObjectId() const;

    // declarative fashion api
    // the index operators are declared in BeamlineNode but only work when the BeamlineNode is a group. defined here so they can be called without
    // casting an object to Group

    virtual const BeamlineNode* operator[](size_t index) const;
    virtual BeamlineNode* operator[](size_t index);

    virtual const BeamlineNode* operator[](const std::string& name) const;
    virtual BeamlineNode* operator[](const std::string& name);

    const Group* asGroup() const;
    Group* asGroup();

    const DesignSource* asSource() const;
    DesignSource* asSource();

    const DesignElement* asElement() const;
    DesignElement* asElement();

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
