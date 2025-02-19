#include "Beamline.h"

#include <stack>
#include <stdexcept>
#include <sstream>

#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Debug/Instrumentor.h"

namespace RAYX {

// Move constructor
Group::Group(Group&& other) noexcept
    : m_position(std::move(other.m_position)), m_orientation(std::move(other.m_orientation)), m_children(std::move(other.m_children)) {
    for (auto& child : m_children) {
        child->m_Parent = this;
    }
}

// Move assignment operator
Group& Group::operator=(Group&& other) noexcept {
    if (this != &other) {
        m_position = std::move(other.m_position);
        m_orientation = std::move(other.m_orientation);
        m_children = std::move(other.m_children);

        for (auto& child : m_children) {
            child->m_Parent = this;
        }
    }
    return *this;
}

// Deep-copy (clone) implementation.
std::unique_ptr<BeamlineNode> Group::clone() const {
    auto copy = std::make_unique<Group>();
    copy->setPosition(m_position);
    copy->setOrientation(m_orientation);
    for (const auto& child : m_children) {
        assert(child && "m_children contains a nullptr!");
        copy->addChild(child->clone());
    }
    return copy;
}

void Group::ctraverse(const std::function<bool(const BeamlineNode&)>& callback) const {
    for (const auto& child : m_children) {
        assert(child && "m_children contains a nullptr!");
        if (callback(*child)) {
            return;
        }
        if (child->isGroup()) {
            auto groupPtr = static_cast<Group*>(child.get());
            groupPtr->ctraverse(callback);
        }
    }
}

void Group::traverse(const std::function<bool(BeamlineNode&)>& callback) {
    for (auto& child : m_children) {
        assert(child && "m_children contains a nullptr!");
        if (callback(*child)) {
            return;
        }
        assert(child && "m_children contains a nullptr! This should never happen.");
        if (child->isGroup()) {
            auto groupPtr = static_cast<Group*>(child.get());
            groupPtr->traverse(callback);
        }
    }
}

// Add a child node.
void Group::addChild(std::unique_ptr<BeamlineNode> child) {
    assert(child && "Attempted to add a nullptr child to Group!");
    if (!child) return;
    child->m_Parent = this;
    m_children.push_back(std::move(child));
}

MaterialTables Group::calcMinimalMaterialTables() const {
    auto elements = getElements();
    std::array<bool, 92> relevantMaterials{};
    relevantMaterials.fill(false);
    for (const auto& elemPtr : elements) {
        int material = static_cast<int>(elemPtr->getMaterial());  // assuming getMaterial() exists
        if (material >= 1 && material <= 92) {
            relevantMaterials[material - 1] = true;
        }
    }
    return loadMaterialTables(relevantMaterials);
}

void Group::accumulateLightSourcesWorldPositions(const Group& group, const glm::dvec4& parentPos, const glm::dmat4& parentOri,
                                                 std::vector<glm::dvec4>& positions) {
    glm::dvec4 currentPos = parentOri * group.getPosition() + parentPos;
    glm::dmat4 currentOri = parentOri * group.getOrientation();

    for (const auto& child : group) {
        assert(child && "m_children contains a nullptr!");
        if (child->isSource()) {
            DesignSource* srcPtr = static_cast<DesignSource*>(child.get());
            glm::dvec4 worldPos = currentOri * srcPtr->getPosition() + currentPos;
            positions.push_back(worldPos);
        } else if (child->isGroup()) {
            Group* grpPtr = static_cast<Group*>(child.get());
            if (grpPtr) {
                accumulateLightSourcesWorldPositions(*grpPtr, currentPos, currentOri, positions);
            }
        }
    }
}

std::vector<OpticalElement> Group::compileElements() const {
    std::vector<OpticalElement> elements;

    auto recurse = [&](auto& self, const Group& grp, const glm::dvec4& parentPos, const glm::dmat4& parentOri) -> void {
        glm::dvec4 thisGroupPos = parentOri * grp.getPosition() + parentPos;
        glm::dmat4 thisGroupOri = parentOri * grp.getOrientation();

        for (const auto& child : grp.m_children) {
            assert(child && "m_children contains a nullptr!");
            if (child->isElement()) {
                const auto* dePtr = static_cast<DesignElement*>(child.get());
                elements.push_back(dePtr->compile(thisGroupPos, thisGroupOri));
            } else if (child->isGroup()) {
                const auto* groupPtr = static_cast<Group*>(child.get());
                self(self, *groupPtr, thisGroupPos, thisGroupOri);
            }  // Ignore DesignSources
        }
    };

    // Start recursion at this group
    recurse(recurse, *this, glm::dvec4(0, 0, 0, 1), glm::dmat4(1.0));
    return elements;
}

std::vector<Ray> Group::compileSources(int thread_count) const {
    RAYX_PROFILE_FUNCTION_STDOUT();
    std::vector<Ray> rays;

    auto traverseWithTransforms = [&rays, thread_count](const Group& group, const glm::dvec4& parentPosition, const glm::dmat4& parentOrientation,
                                                        const auto& self) -> void {
        // Compute the transform for this group
        glm::dvec4 currentPosition = parentOrientation * group.getPosition() + parentPosition;
        glm::dmat4 currentOrientation = parentOrientation * group.getOrientation();

        for (const auto& child : group.m_children) {  // For each child...
            assert(child && "m_children contains a nullptr!");
            if (child->isSource()) {
                const auto* srcPtr = static_cast<DesignSource*>(child.get());
                auto sourceRays = srcPtr->compile(thread_count, currentPosition, currentOrientation);
                for (auto& ray : sourceRays) {
                    ray.m_sourceID = static_cast<uint32_t>(rays.size());
                }
                rays.insert(rays.end(), sourceRays.begin(), sourceRays.end());
            } else if (child->isGroup()) {
                const auto* childGroupPtr = static_cast<Group*>(child.get());
                self(*childGroupPtr, currentPosition, currentOrientation, self);
            }
        }
    };

    traverseWithTransforms(*this, glm::dvec4(0, 0, 0, 1), glm::dmat4(1), traverseWithTransforms);
    return rays;
}

std::vector<const DesignElement*> Group::getElements() const {
    std::vector<const DesignElement*> elements;
    ctraverse([&elements](const BeamlineNode& node) -> bool {
        if (node.isElement()) {
            elements.push_back(static_cast<const DesignElement*>(&node));
        }
        return false;
    });
    return elements;
}

std::vector<const DesignSource*> Group::getSources() const {
    std::vector<const DesignSource*> sources;
    ctraverse([&sources](const BeamlineNode& node) -> bool {
        if (node.isSource()) {
            sources.push_back(static_cast<const DesignSource*>(&node));
        }
        return false;
    });
    return sources;
}

size_t Group::numElements() const {
    size_t count = 0;
    ctraverse([&count](const BeamlineNode& node) -> bool {
        if (node.isElement()) {
            ++count;
        }
        return false;
    });
    return count;
}

size_t Group::numSources() const {
    size_t count = 0;
    ctraverse([&count](const BeamlineNode& node) -> bool {
        if (node.isSource()) {
            ++count;
        }
        return false;
    });
    return count;
}

}  // namespace RAYX
