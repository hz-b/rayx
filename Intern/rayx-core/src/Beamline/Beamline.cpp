#include "Beamline.h"
#include <stdexcept>

#include "Design/DesignSource.h"
#include "Design/DesignElement.h"
#include "Debug/Instrumentor.h"

namespace RAYX {

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

// Implementation of Group's getNodeType
NodeType Group::getNodeType() const {
    // A Group is inherently of NodeType::Group
    return NodeType::Group;
}

// Implementation of getNode to access a child node by index
const BeamlineNode& Group::getNode(size_t index) const {
    if (index >= children.size()) {
        throw std::out_of_range("Index out of range in Group::getNode");
    }
    return children[index];
}

void Group::traverse(const std::function<void(const BeamlineNode&)>& callback) const {
    // Apply the callback to each child
    for (const auto& child : children) {
        callback(child);
        // If the child is a Group, recursively traverse it
        if (std::holds_alternative<Group>(child)) {
            std::get<Group>(child).traverse(callback);
        }
    }
}

// Add a child node (move semantics)
void Group::addChild(BeamlineNode&& child) { children.push_back(std::move(child)); }

MaterialTables Group::calcMinimalMaterialTables() const {
    std::vector<const DesignElement*> elements = getElements();

    std::array<bool, 92> relevantMaterials{};
    relevantMaterials.fill(false);

    for (const auto* e : elements) {
        int material = static_cast<int>(e->getMaterial());  // in [1, 92]
        if (1 <= material && material <= 92) {
            relevantMaterials[material - 1] = true;
        }
    }

    return loadMaterialTables(relevantMaterials);
}

std::vector<OpticalElement> Group::compileElements() const {
    std::vector<OpticalElement> elements;

    // We start at the "world" identity if top-level
    auto recurse = [&](auto& self, const Group& grp, const glm::dvec4& parentPos, const glm::dmat4& parentOri) -> void {
        // Compute *this group�s* global transform by applying parent transform
        glm::dvec4 thisGroupPos = parentOri * grp.getPosition() + parentPos;
        glm::dmat4 thisGroupOri = parentOri * grp.getOrientation();

        // Recurse children
        for (const auto& child : grp.children) {
            if (std::holds_alternative<DesignElement>(child)) {
                // Pass parent's global transform (thisGroupPos, thisGroupOri)
                elements.push_back(std::get<DesignElement>(child).compile(thisGroupPos, thisGroupOri));
            } else if (std::holds_alternative<Group>(child)) {
                // Recurse deeper
                self(self, std::get<Group>(child), thisGroupPos, thisGroupOri);
            }
        }
    };

    // Start recursion with identity
    recurse(recurse, *this, glm::dvec4(0, 0, 0, 1), glm::dmat4(1.0));
    return elements;
}

std::vector<Ray> Group::compileSources(int thread_count) const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    std::vector<Ray> rays;

    // Recursive traversal with group transformations
    auto traverseWithTransforms = [&rays, thread_count](const Group& group, const glm::dvec4& parentPosition, const glm::dmat4& parentOrientation,
                                                        const auto& self) -> void {
        // Accumulate group transformations
        glm::dvec4 currentPosition = parentOrientation * group.getPosition() + parentPosition;
        glm::dmat4 currentOrientation = parentOrientation * group.getOrientation();

        for (const auto& child : group.children) {
            if (std::holds_alternative<DesignSource>(child)) {
                // Compile the source with the accumulated transformations
                auto sourceRays = std::get<DesignSource>(child).compile(thread_count, currentPosition, currentOrientation);
                // Update source IDs for rays
                for (auto& ray : sourceRays) {
                    ray.m_sourceID = static_cast<uint32_t>(rays.size());
                }
                // Add rays to the main list
                rays.insert(rays.end(), sourceRays.begin(), sourceRays.end());
            } else if (std::holds_alternative<Group>(child)) {
                // Recurse into the child group
                self(std::get<Group>(child), currentPosition, currentOrientation, self);
            }
        }
    };

    // Start traversal with identity transformations
    traverseWithTransforms(*this, glm::dvec4(0, 0, 0, 1), glm::dmat4(1), traverseWithTransforms);

    return rays;
}

// Retrieve all DesignElements (deep)
std::vector<const DesignElement*> Group::getElements() const {
    std::vector<const DesignElement*> elements;
    traverse([&elements](const BeamlineNode& node) {
        if (std::holds_alternative<DesignElement>(node)) {
            elements.push_back(&std::get<DesignElement>(node));
        }
    });
    return elements;
}

// Retrieve all DesignSources (deep)
std::vector<const DesignSource*> Group::getSources() const {
    std::vector<const DesignSource*> sources;
    traverse([&sources](const BeamlineNode& node) {
        if (std::holds_alternative<DesignSource>(node)) {
            sources.push_back(&std::get<DesignSource>(node));
        }
    });
    return sources;
}

size_t Group::numElements() const {
    size_t count = 0;
    traverse([&count](const BeamlineNode& node) {
        if (std::holds_alternative<DesignElement>(node)) {
            count++;
        }
    });
    return count;
}

size_t Group::numSources() const {
    size_t count = 0;
    traverse([&count](const BeamlineNode& node) {
        if (std::holds_alternative<DesignSource>(node)) {
            count++;
        }
    });
    return count;
}

// Retrieve all Groups (deep)
std::vector<const Group*> Group::getGroups() const {
    std::vector<const Group*> groups;
    traverse([&groups](const BeamlineNode& node) {
        if (std::holds_alternative<Group>(node)) {
            groups.push_back(&std::get<Group>(node));
        }
    });
    return groups;
}

}  // namespace RAYX