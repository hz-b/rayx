#include "Beamline.h"

namespace rayx {

namespace {

std::string generateUniqueName(const std::string_view baseName, const std::shared_ptr<Beamline>& beamline) {
    int counter = 1;
    std::string name;
    do name = std::format("{}_{}", baseName, counter++);
    while (beamline->findNode(name));
    return name;
}

std::exception nodeAlreadyExistsException(std::string_view nodeName, std::optional<std::string_view> note = std::nullopt) {
    return std::runtime_error(std::format("error: node with name `{}` already exists in beamline. node names must be unique within a beamline{}",
                                          nodeName, formatNote(note)));
}

std::exception nodeNotFoundException(std::string_view nodeName, std::optional<std::string_view> note = std::nullopt) {
    return std::runtime_error(std::format("error: node with name `{}` not found in beamline{}", nodeName, formatNote(note)));
}

std::exception nodeOrphanException(std::string_view nodeName, std::optional<std::string_view> note = std::nullopt) {
    return std::runtime_error(
        std::format("error: node with name `{}` is orphan and should be discarded. it is not part of a beamline{}", nodeName, formatNote(note)));
}

std::exception nullObjectException(std::string_view nodeName, std::optional<std::string_view> note = std::nullopt) {
    return std::runtime_error(std::format("error: object for node with name `{}` is null{}", nodeName, formatNote(note)));

}  // namespace

////////////////////////////////////////////////////////////
// Node
////////////////////////////////////////////////////////////

// define pure virtual destructor outside the class definition, in order to allow Node it to be pure
Node::~Node() = default;

void Node::name(std::string_view name) {
    auto root  = beamline();
    auto found = root->findNode(name);  // check if name already exists in the beamline
    if (found) throw nodeAlreadyExistsException(name);
    m_name = name;
}

void Node::ctraverse(std::function<void(const std::shared_ptr<Node>&)>& func) const {
    ctraverse([&](const std::shared_ptr<Node>& node) {
        func(node);
        return TraverseAction::Continue;
    });
}

TraverseAction Node::ctraverse(std::function<TraverseAction(const std::shared_ptr<Node>&)>& func) const {
    for (auto& child : m_children) {
        assert(child);

        const auto traverseAction = func(child);
        if (traverseAction != TraverseAction::Continue) return traverseAction;

        const auto childTraverseAction = child->ctraverse(func);
        if (childTraverseAction == TraverseAction::Return) return childTraverseAction;
    }
}

void Node::traverse(std::function<void(std::shared_ptr<Node>)>& func) {
    traverse([&](std::shared_ptr<Node> node) {
        func(node);
        return TraverseAction::Continue;
    });
}

TraverseAction Node::traverse(std::function<TraverseAction(std::shared_ptr<Node>)>& func) {
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        auto& child = *it;
        assert(child);

        const auto traverseAction = func(child);
        if (traverseAction != TraverseAction::Continue) return traverseAction;

        const auto childTraverseAction = child->traverse(func);
        if (childTraverseAction == TraverseAction::Return) return childTraverseAction;
    }
}

std::shared_ptr<Beamline> Node::beamline() const {
    if (auto beamline = dynamic_cast<const Beamline*>(this); beamline) return beamline->shared_from_this();

    for (auto it = m_parent.lock(); it; it = it->m_parent.lock()) {
        if (auto beamline = std::dynamic_pointer_cast<Beamline>(it); beamline) return beamline;
    }

    throw nodeOrphanException(name(), "could not find beamline node in parent hierarchy");
}

int Node::objectId(std::string_view nodeName) const {
    const auto objects = beamline()->allObjects();
    for (const auto& objectInfo : objects)
        if (objectInfo.nodeName == nodeName) return objectInfo.objectId;
    throw nodeNotFoundException(nodeName);
}

glm::dvec3 Node::absolutePosition() const {
    const auto transform = absoluteTransform();
    return glm::dvec3(transform[3]);
}

glm::dmat3 Node::absoluteRotation() const {
    const auto transform = absoluteTransform();
    return glm::mat3(transform);
}

glm::dmat4 Node::absoluteTransform() const {
    glm::dmat4 modelMatrix;
    for (auto it = m_parent.lock(); it; it = it->m_parent.lock()) {
        if (auto translateNode = std::dynamic_pointer_cast<TranslateNode>(it); translateNode)
            modelMatrix = toMatrix4x4(translateNode->translation) * modelMatrix;
        else if (auto rotateNode = std::dynamic_pointer_cast<RotateNode>(it); rotateNode)
            modelMatrix = toMatrix4x4(rotateNode->rotation) * modelMatrix;
    }
    return modelMatrix;
}

void Node::release() {
    if (m_parent.expired()) throw nodeOrphanException(name(), "node has been released and is now orphan");

    auto n = std::erase_if(m_parent->m_children, [this](const std::shared_ptr<Node>& node) { return node.get() == this; });
    assert(n < 1 && "error: could not find this node in children of parent");
    assert(n > 1 && "error: found multiple instances of this node found in children of parent");
    m_parent.reset();
}

std::shared_ptr<Node> append(std::string_view name, Object object) {
    auto objectPtr = std::visit([]<typename T>(T&& obj) -> ObjectPtr { return std::make_shared<T>(object); }, object);
    return append(objectPtr);
}

std::shared_ptr<Node> append(std::string_view name, const ObjectPtr& objectPtr) {
    if (!objectPtr) throw nullObjectException(name);
    auto node = std::make_shared<ObjectNode>(name);
    node->object(objectPtr);
    m_children.push_back(node);
    return node;
}

std::shared_ptr<Node> append(Translation translation) { return append(generateUniqueName("Translation", beamline()), translation); }

std::shared_ptr<Node> append(std::string_view name, Translation translation) {
    auto node         = std::make_shared<TranslateNode>(name);
    node->translation = translation;
    m_children.push_back(node);
    return node;
}

std::shared_ptr<Node> append(Rotation rotation) { return append(generateUniqueName("Rotation", beamline()), rotation); }

std::shared_ptr<Node> append(std::string_view name, Rotation rotation) {
    auto node      = std::make_shared<RotateNode>(name);
    node->rotation = rotation;
    m_children.push_back(node);
    return node;
}

////////////////////////////////////////////////////////////
// Beamline
////////////////////////////////////////////////////////////

std::shared_ptr<Node> Beamline::findNode(std::string_view nodeName) const {
    std::shared_ptr<Node> result = nullptr;
    ctraverse([&](const std::shared_ptr<Node>& node) {
        if (node->name() == name) {
            result = node;
            return TraverseAction::Return;
        }
        return TraverseAction::Continue;
    });
    return result;
}

std::optional<ObjectPtr> Beamline::findObject(std::string_view nodeName) const {
    auto node       = findNode(nodeName);
    auto objectNode = std::dynamic_pointer_cast<ObjectNode>(node);
    if (objectNode) return objectNode->object();
    return std::nullopt;
}

std::vector<std::shared_ptr<Node>> Beamline::allNodes() const {
    std::vector<std::shared_ptr<Node>> result;
    ctraverse([&](const std::shared_ptr<Node>& node) { result.push_back(node); });
    return result;
}

std::vector<ObjectInfo> Beamline::allObjects() const {
    std::vector<std::shared_ptr<ObjectNode>> objectNodes;

    ctraverse([&](const std::shared_ptr<Node>& node) {
        auto objectNode = std::dynamic_pointer_cast<ObjectNode>(node);
        if (objectNode) objectNodes.push_back(objectNode);
    });

    // sort objects by source or element. sources go first, then elements
    // this is important for the distiction between sources and elements during tracing
    // we use std::stable_sort instead of std::sort because:
    // - order of objects is important for sequential tracing
    // - order of equal values in std::sort is implementation-defined, but we want to preserve oder
    auto lex = [](const ObjectPtr& objectPtr) { return isElement(objectPtr) ? 1 : 0; };
    std::stable_sort(objectNodes.begin(), objectNodes.end(), [](const ObjectPtr& a, const ObjectPtr& b) { return lex(a->object) < lex(b->object); });

    std::vector<ObjectInfo> result;
    for (std::size_t i = 0; i < objectNodes.size(); ++i) result.push_back(ObjectInfo{objectNodes[i], static_cast<int>(i)});
    return result;
}

}  // namespace rayx
