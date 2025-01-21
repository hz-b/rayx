#include "Beamline.h"
#include <stdexcept>

#include "Design/DesignSource.h"
#include "Design/DesignElement.h"
#include "Debug/Instrumentor.h"

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

std::vector<Ray> Group::getInputRays(int thread_count) const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    std::vector<DesignSource> sources = getSources();

    if (sources.size() == 0) {
        return {};
    }

    // count number of rays.
    uint32_t raycount = 0;

    for (DesignSource dSource : sources) {
        raycount += (uint32_t)dSource.getNumberOfRays();
    }

    // We add all remaining rays into the rays of the first light source.
    // This is efficient because in most cases there is just one light source, and hence copying them again is unnecessary.
    std::vector<Ray> list = sources[0].compile(thread_count);
    for (Ray& r : list) {
        r.m_sourceID = 0;  // the first light source has ID 0.
    }

    if (sources.size() > 1) {
        list.reserve(raycount);

        for (size_t i = 1; i < sources.size(); i++) {
            std::vector<Ray> sub = sources[i].compile(thread_count);
            for (Ray& r : sub) {
                r.m_sourceID = static_cast<double>(i);
            }
            list.insert(list.end(), sub.begin(), sub.end());
        }
    }
    return list;
}

MaterialTables Group::calcMinimalMaterialTables() const {
    std::vector<DesignElement> elements = getElements();

    std::array<bool, 92> relevantMaterials{};
    relevantMaterials.fill(false);

    for (const auto& e : elements) {
        int material = static_cast<int>(e.getMaterial());  // in [1, 92]
        if (1 <= material && material <= 92) {
            relevantMaterials[material - 1] = true;
        }
    }

    return loadMaterialTables(relevantMaterials);
}

std::vector<OpticalElement> Group::compile() const {
    std::vector<OpticalElement> elements;
    traverse([&elements](const BeamlineNode& node) {
        if (std::holds_alternative<DesignElement>(node)) {
            elements.push_back(std::get<DesignElement>(node).compile());
        }
    });
    return elements;
}

// Retrieve all DesignElements (deep)
std::vector<DesignElement> Group::getElements() const {
    std::vector<DesignElement> elements;
    traverse([&elements](const BeamlineNode& node) {
        if (std::holds_alternative<DesignElement>(node)) {
            elements.push_back(std::get<DesignElement>(node));
        }
    });
    return elements;
}

// Retrieve all DesignSources (deep)
std::vector<DesignSource> Group::getSources() const {
    std::vector<DesignSource> sources;
    traverse([&sources](const BeamlineNode& node) {
        if (std::holds_alternative<DesignSource>(node)) {
            sources.push_back(std::get<DesignSource>(node));
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
std::vector<Group> Group::getGroups() const {
    std::vector<Group> groups;
    traverse([&groups](const BeamlineNode& node) {
        if (std::holds_alternative<Group>(node)) {
            groups.push_back(std::get<Group>(node));
        }
    });
    return groups;
}

}  // namespace RAYX