#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

#include "Object.h"
#include "Rotation.h"
#include "Translation.h"

namespace rayx {

enum class TraverseAction {
    Continue,
    Break,
    Return,
};

struct ObjectNode;
struct TranslateNode;
struct RotateNode;
struct Beamline;

struct Node {
    virtual ~Node()                                                   = 0;
    virtual std::weak_ptr<Node> weak_from_this_base()                 = 0;
    virtual std::weak_ptr<const Node> weak_from_this_base() const     = 0;
    virtual std::shared_ptr<Node> shared_from_this_base()             = 0;
    virtual std::shared_ptr<const Node> shared_from_this_base() const = 0;

    std::string name() const { return m_name; }
    void name(std::string_view name);

    ////////////////////////////////////////////////////////////
    // accessors
    ////////////////////////////////////////////////////////////

    bool isOrphan() const;
    std::shared_ptr<Node> parent() const { return m_parent.lock(); }
    std::vector<std::shared_ptr<Node>> children() const { return m_children; }

    // iteration
    //
    // adding nodes during traversal is not allowed
    TraverseAction ctraverse(std::function<TraverseAction(const std::shared_ptr<Node>&)> func) const;
    // iterate backward in order to allow the user to delete nodes along the way
    TraverseAction traverse(std::function<TraverseAction(std::shared_ptr<Node>)> func);

    // object accessors
    int objectId() const;

    // transform accessors
    glm::dvec3 absolutePosition() const;
    glm::dmat3 absoluteRotation() const;
    glm::dmat4 absoluteTransform() const;

    ////////////////////////////////////////////////////////////
    // modifiers
    ////////////////////////////////////////////////////////////

    std::shared_ptr<Node> append(std::string_view name, Object object);
    std::shared_ptr<Node> append(std::string_view name, const ObjectPtr& object);
    std::shared_ptr<Node> append(Translation translation);
    std::shared_ptr<Node> append(std::string_view name, Translation translation);
    std::shared_ptr<Node> append(Rotation rotation);
    std::shared_ptr<Node> append(std::string_view name, Rotation rotation);

    void release();

  protected:
    Node(std::string_view name) : m_name(name) {}

    std::string m_name;
    std::vector<std::shared_ptr<Node>> m_children;
    std::weak_ptr<Node> m_parent;

  private:
    std::shared_ptr<const Beamline> beamline() const;
    std::shared_ptr<Beamline> beamline();
};

struct ObjectNode : Node, std::enable_shared_from_this<ObjectNode> {
    ObjectNode(std::string_view name, ObjectPtr object) : Node(name) { this->object(object); }

    std::weak_ptr<Node> weak_from_this_base() { return weak_from_this(); }
    std::weak_ptr<const Node> weak_from_this_base() const { return weak_from_this(); }
    std::shared_ptr<Node> shared_from_this_base() { return shared_from_this(); }
    std::shared_ptr<const Node> shared_from_this_base() const { return shared_from_this(); }

    RAYX_VALIDATED_PROPERTY(ObjectNode, ObjectPtr, object, detail::validateNotNull);
};

struct TranslateNode : Node, std::enable_shared_from_this<TranslateNode> {
    TranslateNode(std::string_view name, Translation translation) : Node(name), translation(translation) {}

    std::weak_ptr<Node> weak_from_this_base() { return weak_from_this(); }
    std::weak_ptr<const Node> weak_from_this_base() const { return weak_from_this(); }
    std::shared_ptr<Node> shared_from_this_base() { return shared_from_this(); }
    std::shared_ptr<const Node> shared_from_this_base() const { return shared_from_this(); }

    Translation translation;
};

struct RotateNode : Node, std::enable_shared_from_this<RotateNode> {
    RotateNode(std::string_view name, Rotation rotation) : Node(name), rotation(rotation) {}

    std::weak_ptr<Node> weak_from_this_base() { return weak_from_this(); }
    std::weak_ptr<const Node> weak_from_this_base() const { return weak_from_this(); }
    std::shared_ptr<Node> shared_from_this_base() { return shared_from_this(); }
    std::shared_ptr<const Node> shared_from_this_base() const { return shared_from_this(); }

    Rotation rotation;
};

struct ObjectInfo {
    std::shared_ptr<ObjectNode> node;
    int objectId;
};

struct Beamline : Node, std::enable_shared_from_this<Beamline> {
    Beamline(std::string_view name) : Node(name) {}

    std::weak_ptr<Node> weak_from_this_base() { return weak_from_this(); }
    std::weak_ptr<const Node> weak_from_this_base() const { return weak_from_this(); }
    std::shared_ptr<Node> shared_from_this_base() { return shared_from_this(); }
    std::shared_ptr<const Node> shared_from_this_base() const { return shared_from_this(); }

    ////////////////////////////////////////////////////////////
    // node accessors
    ////////////////////////////////////////////////////////////

    // tree accessors
    std::shared_ptr<Node> findNode(std::string_view nodeName) const;
    std::optional<ObjectPtr> findObject(std::string_view nodeName) const;
    std::vector<std::shared_ptr<Node>> allNodes() const;
    std::vector<ObjectInfo> allObjects() const;
};

}  // namespace rayx
