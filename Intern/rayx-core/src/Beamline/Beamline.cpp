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
    : m_position(std::move(other.m_position)), m_orientation(std::move(other.m_orientation)), m_children(std::move(other.m_children)) {}

// Move assignment operator
Group& Group::operator=(Group&& other) noexcept {
    if (this != &other) {
        m_position = std::move(other.m_position);
        m_orientation = std::move(other.m_orientation);
        m_children = std::move(other.m_children);
    }
    return *this;
}

// Deep-copy (clone) implementation.
Group Group::clone() const {
    Group copy;
    copy.setPosition(m_position);
    copy.setOrientation(m_orientation);
    // For each child, clone it and add a new shared pointer.
    for (const auto& child : m_children) {
        std::visit(
            [&copy](auto& ptr) {
                using T = std::decay_t<decltype(ptr)>;
                if constexpr (std::is_same_v<T, std::shared_ptr<DesignElement>>) {
                    copy.addChild(BeamlineNode(std::make_shared<DesignElement>(ptr->clone())));
                } else if constexpr (std::is_same_v<T, std::shared_ptr<DesignSource>>) {
                    copy.addChild(BeamlineNode(std::make_shared<DesignSource>(ptr->clone())));
                } else if constexpr (std::is_same_v<T, std::shared_ptr<Group>>) {
                    copy.addChild(BeamlineNode(std::make_shared<Group>(ptr->clone())));
                }
            },
            child);
    }
    return copy;
}

template <typename Callback>
void Group::traverse(Callback&& callback) const {
    for (const auto& child : m_children) {
        callback(child);
        if (auto* groupPtr = std::get_if<std::unique_ptr<Group>>(&child)) {
            if (*groupPtr) (*groupPtr)->traverse(std::forward<Callback>(callback));
        }
    }
}

// A Group is always a Group.
NodeType Group::getNodeType() const { return NodeType::Group; }

// Add a child node.
void Group::addChild(BeamlineNode&& child) { m_children.push_back(std::move(child)); }

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
        if (std::holds_alternative<std::unique_ptr<DesignSource>>(child)) {
            auto& srcPtr = std::get<std::unique_ptr<DesignSource>>(child);
            if (srcPtr) {
                glm::dvec4 worldPos = currentOri * srcPtr->getPosition() + currentPos;
                positions.push_back(worldPos);
            }
        } else if (std::holds_alternative<std::unique_ptr<Group>>(child)) {
            auto& childGroupPtr = std::get<std::unique_ptr<Group>>(child);
            if (childGroupPtr) {
                accumulateLightSourcesWorldPositions(*childGroupPtr, currentPos, currentOri, positions);
            }
        }
    }
}

std::vector<OpticalElement> Group::compileElements() const {
    std::vector<OpticalElement> elements;

    auto recurse = [&](auto& self, const Group& grp, const glm::dvec4& parentPos, const glm::dmat4& parentOri) -> void {
        glm::dvec4 thisGroupPos = parentOri * grp.getPosition() + parentPos;
        glm::dmat4 thisGroupOri = parentOri * grp.getOrientation();

        for (const auto& child : grp.m_children) {  // For each child...
            if (std::holds_alternative<std::unique_ptr<DesignElement>>(child)) {
                const auto& dePtr = std::get<std::unique_ptr<DesignElement>>(child);
                if (dePtr) {
                    // Compile an OpticalElement from the DesignElement
                    elements.push_back(dePtr->compile(thisGroupPos, thisGroupOri));
                }
            } else if (std::holds_alternative<std::unique_ptr<Group>>(child)) {
                const auto& groupPtr = std::get<std::unique_ptr<Group>>(child);
                if (groupPtr) {
                    // Recurse into the child group
                    self(self, *groupPtr, thisGroupPos, thisGroupOri);
                }
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
            if (std::holds_alternative<std::unique_ptr<DesignSource>>(child)) {
                const auto& srcPtr = std::get<std::unique_ptr<DesignSource>>(child);
                if (srcPtr) {
                    // Compile the rays for this source
                    auto sourceRays = srcPtr->compile(thread_count, currentPosition, currentOrientation);
                    for (auto& ray : sourceRays) {
                        ray.m_sourceID = static_cast<uint32_t>(rays.size());
                    }
                    rays.insert(rays.end(), sourceRays.begin(), sourceRays.end());
                }
            } else if (std::holds_alternative<std::unique_ptr<Group>>(child)) {
                const auto& childGroupPtr = std::get<std::unique_ptr<Group>>(child);
                if (childGroupPtr) {
                    self(*childGroupPtr, currentPosition, currentOrientation, self);
                }
            }
        }
    };

    traverseWithTransforms(*this, glm::dvec4(0, 0, 0, 1), glm::dmat4(1), traverseWithTransforms);
    return rays;
}

std::vector<DesignElement*> Group::getElements() const {
    std::vector<DesignElement*> elements;
    for (const auto& node : m_children) {
        if (std::holds_alternative<std::unique_ptr<DesignElement>>(node)) {
            elements.push_back(std::get<std::unique_ptr<DesignElement>>(node).get());
        }
    }
    return elements;
}

std::vector<DesignSource*> Group::getSources() const {
    std::vector<DesignSource*> sources;
    for (const auto& node : m_children) {
        if (std::holds_alternative<std::unique_ptr<DesignSource>>(node)) {
            sources.push_back(std::get<std::unique_ptr<DesignSource>>(node).get());
        }
    }
    return sources;
}

std::vector<Group*> Group::getGroups() const {
    std::vector<Group*> groups;
    for (const auto& node : m_children) {
        if (std::holds_alternative<std::unique_ptr<Group>>(node)) {
            groups.push_back(std::get<std::unique_ptr<Group>>(node).get());
        }
    }
    return groups;
}

size_t Group::numElements() const {
    size_t count = 0;
    traverse([&count](const BeamlineNode& node) {
        if (std::holds_alternative<std::unique_ptr<DesignElement>>(node)) {
            ++count;
        }
    });
    return count;
}

size_t Group::numSources() const {
    size_t count = 0;
    traverse([&count](const BeamlineNode& node) {
        if (std::holds_alternative<std::unique_ptr<DesignSource>>(node)) {
            ++count;
        }
    });
    return count;
}

}  // namespace RAYX
