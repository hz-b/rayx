#include "Node.h"

#include <cassert>

#include "Beamline.h"

namespace RAYX {

glm::dvec4 BeamlineNode::getWorldPosition() const {
    glm::dvec4 localPos = getPosition();

    if (!m_Parent) return localPos;

    assert(m_Parent->isGroup() && "Parent must be a Group!");
    const Group* parentGroup = static_cast<const Group*>(m_Parent);

    glm::dmat4 parentOri = parentGroup->getWorldOrientation();
    glm::dvec4 parentPos = parentGroup->getWorldPosition();
    return parentOri * localPos + parentPos;
}

glm::dmat4 BeamlineNode::getWorldOrientation() const {
    glm::dmat4 ori = getOrientation();

    if (!m_Parent) return ori;

    assert(m_Parent->isGroup() && "Parent must be a Group!");
    const Group* parentGroup = static_cast<const Group*>(m_Parent);

    return parentGroup->getWorldOrientation() * ori;
}

}  // namespace RAYX
