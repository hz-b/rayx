#pragma once

#include <functional>
#include <glm.hpp>
#include <memory>
#include <variant>
#include <vector>

#include "Core.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Node.h"

namespace rayx {

/**
 * A beamline group that can contain child nodes (elements, sources, or other groups).
 * A Group is a building block for the model/scene graph.
 */
class RAYX_API Group : public BeamlineNode {
  public:
    Group();
    Group(std::string name);
    ~Group() = default;

    Group(const Group&)            = delete;
    Group& operator=(const Group&) = delete;

    Group(Group&& other) noexcept;
    Group& operator=(Group&& other) noexcept;

    bool isGroup() const override { return true; }

    // Iterators
    auto begin() { return m_children.begin(); }
    auto end() { return m_children.end(); }
    auto begin() const { return m_children.cbegin(); }
    auto end() const { return m_children.cend(); }
    auto cbegin() const { return m_children.cbegin(); }
    auto cend() const { return m_children.cend(); }

    /**
     * @brief Adds a child to this Group.
     *
     * @param child A unique_ptr to a BeamlineNode to be moved into this Group.
     */
    void addChild(std::unique_ptr<BeamlineNode> child);

    [[nodiscard]] std::unique_ptr<BeamlineNode> releaseNodeFromChildren(const BeamlineNode* node);
    [[nodiscard]] std::unique_ptr<BeamlineNode> releaseNodeFromTree(const BeamlineNode* node);

    size_t numElements() const;
    size_t numSources() const;
    size_t numObjects() const;
    size_t numRayPaths() const;

    // recursive search

    const BeamlineNode* findNodeByName(const std::string& name) const;
    BeamlineNode* findNodeByName(const std::string& name);

    const Group* findGroupByName(const std::string& name) const;
    Group* findGroupByName(const std::string& name);

    const DesignSource* findSourceByName(const std::string& name) const;
    DesignSource* findSourceByName(const std::string& name);

    const DesignElement* findElementByName(const std::string& name) const;
    DesignElement* findElementByName(const std::string& name);

    const BeamlineNode* findNode(const std::function<bool(const BeamlineNode&)>& pred) const;
    BeamlineNode* findNode(const std::function<bool(const BeamlineNode&)>& pred);

    const Group* findGroup(const std::function<bool(const Group&)>& pred) const;
    Group* findGroup(const std::function<bool(const Group&)>& pred);

    const DesignSource* findSource(const std::function<bool(const DesignSource&)>& pred) const;
    DesignSource* findSource(const std::function<bool(const DesignSource&)>& pred);

    const DesignElement* findElement(const std::function<bool(const DesignElement&)>& pred) const;
    DesignElement* findElement(const std::function<bool(const DesignElement&)>& pred);

    /**
     * @brief Creates a deep copy of this group and its children.
     *
     * @return unique_ptr to the base class (can be statically casted)
     */
    std::unique_ptr<BeamlineNode> clone() const override;

    /**
     * @brief Recursively traverse the model/scene graph down from this node.
     *
     * Calls the @p callback function on each node.
     *
     * @param callback Callback function which takes a BeamlineNode reference and returns a bool. Returning true stops the traversal and returning
     * false continues it.
     */
    void ctraverse(const std::function<bool(const BeamlineNode&)>& callback) const;

    /**
     * @brief Recursively traverses this Group and its children (non-const).
     *
     * Calls @p callback on each node (including nested Groups).
     * If the callback returns true at any point, the traversal stops early.
     *
     * @param callback A function that takes a BeamlineNode& and returns
     *                 a bool (true = stop traversal, false = continue).
     */
    void traverse(const std::function<bool(BeamlineNode&)>& callback);

    // TODO: this should not be part of the API
    /**
     * @brief Calculates the minimal set of material tables required by elements in this Group.
     *
     * Gathers the material IDs used by all child DesignElements and merges them
     * into a single MaterialTables object.
     *
     * @return A MaterialTables object with data for all relevant materials.
     */
    MaterialTables calcMinimalMaterialTables() const;

    // TODO: this should not be part of the API
    /**
     * @brief Recursively converts all DesignElement nodes into OpticalElements with full transforms.
     *
     * @return A vector of OpticalElementAndTransform objects compiled from the Group hierarchy.
     */
    std::vector<OpticalElementAndTransform> compileElements() const;

    // TODO: why would we need this? ray-ui uses this function
    /**
     * @brief Gathers the world positions of all light sources within a Group hierarchy.
     *
     * @param group       A reference to the current Group to process.
     * @param parentPos   The position of the parent Group (in world coords).
     * @param parentOri   The orientation of the parent Group (in world coords).
     * @param positions   A reference to a vector of world positions. New positions are appended.
     */
    static void accumulateLightSourcesWorldPositions(const Group& group, const glm::dvec4& parentPos, const glm::dmat4& parentOri,
                                                     std::vector<glm::dvec4>& positions);

    std::vector<std::string> getElementNames() const;
    std::vector<std::string> getSourceNames() const;
    std::vector<std::string> getObjectNames() const;

    std::vector<const DesignElement*> getElements() const;
    std::vector<const DesignSource*> getSources() const;

    const std::vector<std::unique_ptr<BeamlineNode>>& getChildren() const { return m_children; }

    size_t findObjectIdByNode(const BeamlineNode* node) const;
    const BeamlineNode* findNodeByObjectId(const size_t objectId) const;
    BeamlineNode* findNodeByObjectId(const size_t objectId);

  private:
    // m_children vec is not checked for dangling or nullptrs anywhere, changes to the Group interface/implementation are to be made with care
    std::vector<std::unique_ptr<BeamlineNode>> m_children;
};

using Beamline = Group;  // Conceptually, a Beamline is a Group

}  // namespace rayx
