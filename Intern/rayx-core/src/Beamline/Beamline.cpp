#include "Beamline.h"

#include <stdexcept>
#include <sstream>

#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Debug/Instrumentor.h"

namespace RAYX {

// Move constructor
Group::Group(Group&& other) noexcept
    : m_position(std::move(other.m_position)), m_orientation(std::move(other.m_orientation)), children(std::move(other.children)) {}

// Move assignment operator
Group& Group::operator=(Group&& other) noexcept {
    if (this != &other) {
        m_position = std::move(other.m_position);
        m_orientation = std::move(other.m_orientation);
        children = std::move(other.children);
    }
    return *this;
}

// Deep-copy (clone) implementation.
Group Group::clone() const {
    Group copy;
    copy.setPosition(m_position);
    copy.setOrientation(m_orientation);
    // For each child, clone it and add a new shared pointer.
    for (const auto& child : children) {
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

// A Group is always a Group.
NodeType Group::getNodeType() const { return NodeType::Group; }

// Add a child node.
void Group::addChild(BeamlineNode&& child) { children.push_back(std::move(child)); }

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

    traverseGroup(group, [&](const BeamlineNode& node) {
        if (std::holds_alternative<std::shared_ptr<DesignSource>>(node)) {
            const auto& src = std::get<std::shared_ptr<DesignSource>>(node);
            glm::dvec4 worldPos = currentOri * src->getPosition() + currentPos;
            positions.push_back(worldPos);
        } else if (std::holds_alternative<std::shared_ptr<Group>>(node)) {
            const auto& childGroup = std::get<std::shared_ptr<Group>>(node);
            accumulateLightSourcesWorldPositions(*childGroup, currentPos, currentOri, positions);
        }
        // DesignElements are ignored.
    });
}

std::vector<OpticalElement> Group::compileElements() const {
    std::vector<OpticalElement> elements;
    auto recurse = [&](auto& self, const Group& grp, const glm::dvec4& parentPos, const glm::dmat4& parentOri) -> void {
        glm::dvec4 thisGroupPos = parentOri * grp.getPosition() + parentPos;
        glm::dmat4 thisGroupOri = parentOri * grp.getOrientation();
        for (const auto& child : grp.children) {
            if (std::holds_alternative<std::shared_ptr<DesignElement>>(child)) {
                const auto& de = std::get<std::shared_ptr<DesignElement>>(child);
                elements.push_back(de->compile(thisGroupPos, thisGroupOri));
            } else if (std::holds_alternative<std::shared_ptr<Group>>(child)) {
                self(self, *std::get<std::shared_ptr<Group>>(child), thisGroupPos, thisGroupOri);
            }
            // Sources are ignored for optical element compilation.
        }
    };
    recurse(recurse, *this, glm::dvec4(0, 0, 0, 1), glm::dmat4(1.0));
    return elements;
}

std::vector<Ray> Group::compileSources(int thread_count) const {
    RAYX_PROFILE_FUNCTION_STDOUT();
    std::vector<Ray> rays;
    auto traverseWithTransforms = [&rays, thread_count](const Group& group, const glm::dvec4& parentPosition, const glm::dmat4& parentOrientation,
                                                        const auto& self) -> void {
        glm::dvec4 currentPosition = parentOrientation * group.getPosition() + parentPosition;
        glm::dmat4 currentOrientation = parentOrientation * group.getOrientation();

        for (const auto& child : group.children) {
            if (std::holds_alternative<std::shared_ptr<DesignSource>>(child)) {
                const auto& src = std::get<std::shared_ptr<DesignSource>>(child);
                auto sourceRays = src->compile(thread_count, currentPosition, currentOrientation);
                for (auto& ray : sourceRays) {
                    ray.m_sourceID = static_cast<uint32_t>(rays.size());
                }
                rays.insert(rays.end(), sourceRays.begin(), sourceRays.end());
            } else if (std::holds_alternative<std::shared_ptr<Group>>(child)) {
                self(*std::get<std::shared_ptr<Group>>(child), currentPosition, currentOrientation, self);
            }
        }
    };
    traverseWithTransforms(*this, glm::dvec4(0, 0, 0, 1), glm::dmat4(1), traverseWithTransforms);
    return rays;
}

// Retrieve all DesignElements (deep)
std::vector<std::shared_ptr<DesignElement>> Group::getElements() const {
    std::vector<std::shared_ptr<DesignElement>> elements;
    traverseGroup(*this, [&elements](const BeamlineNode& node) {
        if (std::holds_alternative<std::shared_ptr<DesignElement>>(node)) {
            elements.push_back(std::get<std::shared_ptr<DesignElement>>(node));
        }
    });
    return elements;
}

// Retrieve all DesignSources (deep)
std::vector<std::shared_ptr<DesignSource>> Group::getSources() const {
    std::vector<std::shared_ptr<DesignSource>> sources;
    traverseGroup(*this, [&sources](const BeamlineNode& node) {
        if (std::holds_alternative<std::shared_ptr<DesignSource>>(node)) {
            sources.push_back(std::get<std::shared_ptr<DesignSource>>(node));
        }
    });
    return sources;
}

// Retrieve all Groups (deep)
std::vector<std::shared_ptr<Group>> Group::getGroups() const {
    std::vector<std::shared_ptr<Group>> groups;
    traverseGroup(*this, [&groups](const BeamlineNode& node) {
        if (std::holds_alternative<std::shared_ptr<Group>>(node)) {
            groups.push_back(std::get<std::shared_ptr<Group>>(node));
        }
    });
    return groups;
}

size_t Group::numElements() const {
    size_t count = 0;
    traverseGroup(*this, [&count](const BeamlineNode& node) {
        if (std::holds_alternative<std::shared_ptr<DesignElement>>(node)) {
            ++count;
        }
    });
    return count;
}

size_t Group::numSources() const {
    size_t count = 0;
    traverseGroup(*this, [&count](const BeamlineNode& node) {
        if (std::holds_alternative<std::shared_ptr<DesignSource>>(node)) {
            ++count;
        }
    });
    return count;
}

// Non‑const overload.
template <typename Callback>
void traverseGroup(Group& group, Callback&& callback) {
    // Iterate over the mutable children.
    for (auto& child : group.getChildren()) {
        callback(child);
        // If the child is a Group, then recursively traverse it.
        if (std::holds_alternative<std::shared_ptr<Group>>(child)) {
            traverseGroup(*std::get<std::shared_ptr<Group>>(child), callback);
        }
    }
}

// Const overload.
template <typename Callback>
void traverseGroup(const Group& group, Callback&& callback) {
    // Iterate over the children (as read‑only).
    for (const auto& child : group.getChildren()) {
        callback(child);
        // Even in a const context, our variant is defined as holding std::shared_ptr<Group>.
        // We can still call traverseGroup on the pointed-to Group.
        if (std::holds_alternative<std::shared_ptr<Group>>(child)) {
            traverseGroup(*std::get<std::shared_ptr<Group>>(child), callback);
        }
    }
}

}  // namespace RAYX
