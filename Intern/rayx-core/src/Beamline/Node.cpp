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
}  // namespace

glm::dvec4 BeamlineNode::getWorldPosition() const {
    glm::dvec4 localPos = getPosition();

    if (!m_parent) return localPos;

    assert(m_parent->isGroup() && "Parent must be a Group!");
    const Group* parentGroup = static_cast<const Group*>(m_parent);

    glm::dmat4 parentOri = parentGroup->getWorldOrientation();
    glm::dvec4 parentPos = parentGroup->getWorldPosition();
    return parentOri * localPos + parentPos;
}

glm::dmat4 BeamlineNode::getWorldOrientation() const {
    glm::dmat4 ori = getOrientation();

    if (!m_parent) return ori;

    assert(m_parent->isGroup() && "Parent must be a Group!");
    const Group* parentGroup = static_cast<const Group*>(m_parent);

    return parentGroup->getWorldOrientation() * ori;
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

const BeamlineNode* BeamlineNode::operator[](size_t) const {
    throw std::runtime_error(std::format("BeamlineNode of type '{}' cannot be indexed. Only type 'Group' can", getNodeTypeString(this)));
}

BeamlineNode* BeamlineNode::operator[](size_t) {
    throw std::runtime_error(std::format("BeamlineNode of type '{}' cannot be indexed. Only type 'Group' can", getNodeTypeString(this)));
}

const BeamlineNode* BeamlineNode::operator[](const std::string&) const {
    throw std::runtime_error(std::format("BeamlineNode of type '{}' cannot be indexed. Only type 'Group' can", getNodeTypeString(this)));
}

BeamlineNode* BeamlineNode::operator[](const std::string&) {
    throw std::runtime_error(std::format("BeamlineNode of type '{}' cannot be indexed. Only type 'Group' can", getNodeTypeString(this)));
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
