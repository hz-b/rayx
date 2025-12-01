#include "Node.h"

#include <cassert>

#include "Beamline.h"

namespace rayx {

namespace {
std::string getNodeTypeString(const BeamlineNode* node) {
    if (node->isSource()) return "Source";
    if (node->isElement()) return "Element";
    if (node->isGroup()) return "Group";
    RAYX_EXIT << "error: unimpemented BeamlineNode type";
    return "<unimplemented>";
}
}  // unnamed namespace

BeamlineNode::BeamlineNode() : name(createUniqueNodeName()) {}

BeamlineNode(std::string name) : name(std::move(name)) {}

glm::dvec3 BeamlineNode::getWorldSpacePosition() const {
    if (!m_parent) return position;
    return toRotationMatrix(m_parent->asGroup().rotation) * position + m_parent->asGroup().position;
}

Rotation BeamlineNode::getWorldSpaceRotation() const {
    if (!m_parent) return toRotationMatrix(rotation);
    return toRotationMatrix(parent->asGroup()->getWorldSpaceRotation()) * toRotationMatrix(rotation);
}

const BeamlineNode* BeamlineNode::getRoot() const {
    const auto* root = this;
    while (root->m_parent) root = root->m_parent;
    return root;
}

BeamlineNode* BeamlineNode::getRoot() {
    auto* root = this;
    while (root->m_parent) root = root->m_parent;
    return root;
}

int BeamlineNode::getObjectId() const {
    if (!m_parent) return 0;
    return getRoot()->asGroup()->findObjectIdByNode(this);
}

const Group* BeamlineNode::asGroup() const {
    if (!isGroup()) throw std::runtime_error(std::format("BeamlineNode::asGroup() called on '{}' which is not a Group", getNodeTypeString(this)));
    return static_cast<const Group*>(this);
}

Group* BeamlineNode::asGroup() {
    if (!isGroup()) throw std::runtime_error(std::format("BeamlineNode::asGroup() called on '{}' which is not a Group", getNodeTypeString(this)));
    return static_cast<Group*>(this);
}

const DesignSource* BeamlineNode::asSource() const {
    if (!isSource())
        throw std::runtime_error(std::format("BeamlineNode::asSource() called on '{}' which is not a DesignSource", getNodeTypeString(this)));
    return static_cast<const DesignSource*>(this);
}

DesignSource* BeamlineNode::asSource() {
    if (!isSource())
        throw std::runtime_error(std::format("BeamlineNode::asSource() called on '{}' which is not a DesignSource", getNodeTypeString(this)));
    return static_cast<DesignSource*>(this);
}

const DesignElement* BeamlineNode::asElement() const {
    if (!isElement())
        throw std::runtime_error(std::format("BeamlineNode::asElement() called on '{}' which is not a DesignElement", getNodeTypeString(this)));
    return static_cast<const DesignElement*>(this);
}

DesignElement* BeamlineNode::asElement() {
    if (!isElement())
        throw std::runtime_error(std::format("BeamlineNode::asElement() called on '{}' which is not a DesignElement", getNodeTypeString(this)));
    return static_cast<DesignElement*>(this);
}

}  // namespace rayx
