#include "Group.h"
#include <stdexcept>

#include "DesignSource.h"
#include "DesignElement.h"

namespace RAYX {

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

// Add a child node (copy semantics)
void Group::addChild(const BeamlineNode& child) { children.push_back(child); }

// Retrieve all DesignElements (deep)
std::vector<DesignElement> Group::getAllElements() const {
    std::vector<DesignElement> elements;
    traverse([&elements](const BeamlineNode& node) {
        if (std::holds_alternative<DesignElement>(node)) {
            elements.push_back(std::get<DesignElement>(node));
        }
    });
    return elements;
}

// Retrieve all DesignSources (deep)
std::vector<DesignSource> Group::getAllSources() const {
    std::vector<DesignSource> sources;
    traverse([&sources](const BeamlineNode& node) {
        if (std::holds_alternative<DesignSource>(node)) {
            sources.push_back(std::get<DesignSource>(node));
        }
    });
    return sources;
}

// Retrieve all Groups (deep)
std::vector<Group> Group::getAllGroups() const {
    std::vector<Group> groups;
    traverse([&groups](const BeamlineNode& node) {
        if (std::holds_alternative<Group>(node)) {
            groups.push_back(std::get<Group>(node));
        }
    });
    return groups;
}

}  // namespace RAYX