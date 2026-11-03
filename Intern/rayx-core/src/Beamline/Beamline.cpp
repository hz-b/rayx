#include "Beamline.h"

#include <algorithm>
#include <limits>
#include <optional>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <type_traits>

#include "Debug/Instrumentor.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Shader/RefractiveIndex.h"

namespace rayx {

namespace {
std::string getUniqueUnnamedGroupName() {
    static size_t counter = 0;
    return std::format("<unnamed_group_{}>", counter++);
}
}  // unnamed namespace

Group::Group() : m_name(getUniqueUnnamedGroupName()) {}
Group::Group(std::string name) : m_name(std::move(name)) {}

// Move constructor
Group::Group(Group&& other) noexcept
    : m_position(std::move(other.m_position)), m_orientation(std::move(other.m_orientation)), m_children(std::move(other.m_children)) {
    for (auto& child : m_children) { child->m_parent = this; }
}

// Move assignment operator
Group& Group::operator=(Group&& other) noexcept {
    if (this != &other) {
        m_position    = std::move(other.m_position);
        m_orientation = std::move(other.m_orientation);
        m_children    = std::move(other.m_children);

        for (auto& child : m_children) { child->m_parent = this; }
    }
    return *this;
}

// Deep-copy (clone) implementation.
std::unique_ptr<BeamlineNode> Group::clone() const {
    auto copy = std::make_unique<Group>();
    copy->setPosition(m_position);
    copy->setOrientation(m_orientation);
    for (const auto& child : m_children) copy->addChild(child->clone());
    return copy;
}

std::string Group::getName() const { return m_name; }
void Group::setName(std::string name) { m_name = std::move(name); }

const BeamlineNode* Group::findNodeByName(const std::string& name) const {
    const BeamlineNode* result = nullptr;
    ctraverse([&](const BeamlineNode& node) {
        if (node.getName() == name) {
            result = &node;
            return true;
        }
        return false;
    });
    return result;
}

BeamlineNode* Group::findNodeByName(const std::string& name) {
    auto constSelf = const_cast<const Group*>(this);
    return const_cast<BeamlineNode*>(constSelf->findNodeByName(name));
}

const Group* Group::findGroupByName(const std::string& name) const {
    auto node = findNodeByName(name);
    if (node && !node->isGroup()) return nullptr;
    return static_cast<const Group*>(node);
}

Group* Group::findGroupByName(const std::string& name) {
    auto constSelf = const_cast<const Group*>(this);
    return const_cast<Group*>(constSelf->findGroupByName(name));
}

const DesignSource* Group::findSourceByName(const std::string& name) const {
    auto node = findNodeByName(name);
    if (node && !node->isSource()) return nullptr;
    return static_cast<const DesignSource*>(node);
}

DesignSource* Group::findSourceByName(const std::string& name) {
    auto constSelf = const_cast<const Group*>(this);
    return const_cast<DesignSource*>(constSelf->findSourceByName(name));
}

const DesignElement* Group::findElementByName(const std::string& name) const {
    auto node = findNodeByName(name);
    if (node && !node->isElement()) return nullptr;
    return static_cast<const DesignElement*>(node);
}

DesignElement* Group::findElementByName(const std::string& name) {
    auto constSelf = const_cast<const Group*>(this);
    return const_cast<DesignElement*>(constSelf->findElementByName(name));
}

const BeamlineNode* Group::findNode(const std::function<bool(const BeamlineNode&)>& pred) const {
    const BeamlineNode* result = nullptr;
    ctraverse([&](const BeamlineNode& node) {
        if (pred(node)) {
            result = &node;
            return true;
        }
        return false;
    });
    return result;
}

BeamlineNode* Group::findNode(const std::function<bool(const BeamlineNode&)>& pred) {
    auto constSelf = const_cast<const Group*>(this);
    return const_cast<BeamlineNode*>(constSelf->findNode(pred));
}

const Group* Group::findGroup(const std::function<bool(const Group&)>& pred) const {
    const Group* result = nullptr;
    ctraverse([&](const BeamlineNode& node) {
        if (node.isGroup()) {
            const auto* group = static_cast<const Group*>(&node);
            if (pred(*group)) {
                result = group;
                return true;
            }
        }
        return false;
    });
    return result;
}

Group* Group::findGroup(const std::function<bool(const Group&)>& pred) {
    auto constSelf = const_cast<const Group*>(this);
    return const_cast<Group*>(constSelf->findGroup(pred));
}

const DesignSource* Group::findSource(const std::function<bool(const DesignSource&)>& pred) const {
    const DesignSource* result = nullptr;
    ctraverse([&](const BeamlineNode& node) {
        if (node.isSource()) {
            const auto* source = static_cast<const DesignSource*>(&node);
            if (pred(*source)) {
                result = source;
                return true;
            }
        }
        return false;
    });
    return result;
}

DesignSource* Group::findSource(const std::function<bool(const DesignSource&)>& pred) {
    auto constSelf = const_cast<const Group*>(this);
    return const_cast<DesignSource*>(constSelf->findSource(pred));
}

const DesignElement* Group::findElement(const std::function<bool(const DesignElement&)>& pred) const {
    const DesignElement* result = nullptr;
    ctraverse([&](const BeamlineNode& node) {
        if (node.isElement()) {
            const auto* element = static_cast<const DesignElement*>(&node);
            if (pred(*element)) {
                result = element;
                return true;
            }
        }
        return false;
    });
    return result;
}

DesignElement* Group::findElement(const std::function<bool(const DesignElement&)>& pred) {
    auto constSelf = const_cast<const Group*>(this);
    return const_cast<DesignElement*>(constSelf->findElement(pred));
}

const BeamlineNode* Group::operator[](const size_t index) const { return m_children.at(index).get(); }

BeamlineNode* Group::operator[](const size_t index) { return m_children.at(index).get(); }

const BeamlineNode* Group::operator[](const std::string& name) const {
    for (const auto& child : m_children)
        if (child->getName() == name) return child.get();

    throw std::out_of_range(std::format("Group::operator[]: No child with name '{}' found.", name));
}

BeamlineNode* Group::operator[](const std::string& name) {
    for (auto& child : m_children)
        if (child->getName() == name) return child.get();

    throw std::out_of_range(std::format("Group::operator[]: No child with name '{}' found.", name));
}

void Group::ctraverse(const std::function<bool(const BeamlineNode&)>& callback) const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    for (const auto& child : m_children) {
        if (!child) RAYX_EXIT << "m_children contains a nullptr! This should never happen.";
        if (callback(*child)) { return; }
        if (child->isGroup()) {
            auto groupPtr = static_cast<Group*>(child.get());
            groupPtr->ctraverse(callback);
        }
    }
}

void Group::traverse(const std::function<bool(BeamlineNode&)>& callback) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    for (auto& child : m_children) {
        if (!child) RAYX_EXIT << "m_children contains a nullptr! This should never happen.";
        if (callback(*child)) { return; }
        if (child->isGroup()) {
            auto groupPtr = static_cast<Group*>(child.get());
            groupPtr->traverse(callback);
        }
    }
}

// Add a child node.
void Group::addChild(std::unique_ptr<BeamlineNode> node) {
    if (!node) throw std::runtime_error("Attempted to add a nullptr as child to Group!");
    if (node->m_parent) throw std::runtime_error("Attempted to add a BeamlineNode as child to Group, but it already has a parent!");
    node->m_parent = this;
    m_children.push_back(std::move(node));
}

std::unique_ptr<BeamlineNode> Group::releaseNodeFromChildren(const BeamlineNode* node) {
    for (auto it = m_children.begin(); it != m_children.end(); ++it) {
        if (it->get() == node) {
            std::unique_ptr<BeamlineNode> releasedNode = std::move(*it);
            releasedNode->m_parent                     = nullptr;
            m_children.erase(it);
            return releasedNode;
        }
    }

    throw std::runtime_error("Attempted to release a child node that is not part of this Group!");
}

std::unique_ptr<BeamlineNode> Group::releaseNodeFromTree(const BeamlineNode* node) {
    for (auto it = m_children.begin(); it != m_children.end(); ++it) {
        if (it->get() == node) {
            std::unique_ptr<BeamlineNode> releasedNode = std::move(*it);
            releasedNode->m_parent                     = nullptr;
            m_children.erase(it);
            return releasedNode;
        } else if ((*it)->isGroup()) {
            auto groupPtr = static_cast<Group*>((*it).get());
            return groupPtr->releaseNodeFromTree(node);
        }
    }

    throw std::runtime_error("Attempted to release a node that is not part of this Group or its children!");
}

MaterialTables Group::calcMinimalMaterialTables() const {
    auto elements = getElements();
    std::array<bool, 133> relevantMaterials{};
    relevantMaterials.fill(false);
    for (const auto& elemPtr : elements) {
        auto coating = elemPtr->getCoating();
        if (coating.is<detail::CoatingTypes::OneCoating>()) {
            int materialcoating = static_cast<int>(elemPtr->getMaterialCoating());
            if (materialcoating >= 1 && materialcoating <= 133) { relevantMaterials[materialcoating - 1] = true; }
        } else if (coating.is<detail::CoatingTypes::MultilayerCoating>()) {
            auto mlCoating = coating.get<detail::CoatingTypes::MultilayerCoating>();
            for (const auto& mat : mlCoating.material) {
                if (mat >= 1 && mat <= 133) { relevantMaterials[mat - 1] = true; }
            }
        }
        int material = static_cast<int>(elemPtr->getMaterial());  // assuming getMaterial() exists
        if (material >= 1 && material <= 133) { relevantMaterials[material - 1] = true; }
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
            glm::dvec4 worldPos  = currentOri * srcPtr->getPosition() + currentPos;
            positions.push_back(worldPos);
        } else if (child->isGroup()) {
            Group* grpPtr = static_cast<Group*>(child.get());
            if (grpPtr) { accumulateLightSourcesWorldPositions(*grpPtr, currentPos, currentOri, positions); }
        }
    }
}

std::vector<OpticalElementAndTransform> Group::compileElements() const {
    std::vector<OpticalElementAndTransform> elements;

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

namespace {

/// returns the [min, max] energy covered by one energy distribution, if bounded.
std::optional<std::pair<double, double>> energyRangeOf(const EnergyDistributionVariant& en) {
    return std::visit(
        [](const auto& value) -> std::optional<std::pair<double, double>> {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, DatFile>) {
                if (value.m_Lines.empty()) return std::nullopt;
                double lo = std::numeric_limits<double>::max();
                double hi = std::numeric_limits<double>::lowest();
                for (const auto& e : value.m_Lines) {
                    lo = std::min(lo, e.m_energy);
                    hi = std::max(hi, e.m_energy);
                }
                return std::make_pair(lo, hi);
            } else if constexpr (std::is_same_v<T, HardEdge>) {
                return std::make_pair(value.m_centerEnergy - value.m_energySpread / 2.0, value.m_centerEnergy + value.m_energySpread / 2.0);
            } else if constexpr (std::is_same_v<T, SeparateEnergies>) {
                return std::make_pair(value.m_centerEnergy - value.m_energySpread / 2.0, value.m_centerEnergy + value.m_energySpread / 2.0);
            } else if constexpr (std::is_same_v<T, SoftEdge>) {
                // a Gaussian is technically unbounded; use the central +/- 5 sigma window
                const double sigma = value.m_sigma > 0 ? value.m_sigma : 1.0;
                return std::make_pair(value.m_centerEnergy - 5.0 * sigma, value.m_centerEnergy + 5.0 * sigma);
            }
            return std::nullopt;
        },
        en);
}

/// whether the material tables of `material` cover the whole [lo, hi] energy range.
bool materialCoversRange(int material, const MaterialTables& tables, double lo, double hi) {
    if (material < 1 || material > 140) return true;  // vacuum/ideal reflectors need no data
    const int* idx    = tables.indices.data();
    const double* tab = tables.materials.data();

    if (material <= 92) {
        if (getPalikEntryCount(material, idx) > 0) {
            auto first = getPalikEntry(0, material, idx, tab);
            auto last  = getPalikEntry(getPalikEntryCount(material, idx) - 1, material, idx, tab);
            if (first.m_energy <= lo && hi <= last.m_energy) return true;
        }
        if (getNffEntryCount(material, idx) > 0) {
            auto first = getNffEntry(0, material, idx, tab);
            auto last  = getNffEntry(getNffEntryCount(material, idx) - 1, material, idx, tab);
            if (first.m_energy <= lo && hi <= last.m_energy) return true;
        }
        if (getCromerEntryCount(material, idx) > 0) {
            auto first = getCromerEntry(0, material, idx, tab);
            auto last  = getCromerEntry(getCromerEntryCount(material, idx) - 1, material, idx, tab);
            if (first.m_energy <= lo && hi <= last.m_energy) return true;
        }
    } else {
        if (getMolecEntryCount(material, idx) > 0) {
            auto first = getMolecEntry(0, material, idx, tab);
            auto last  = getMolecEntry(getMolecEntryCount(material, idx) - 1, material, idx, tab);
            if (first.m_energy <= lo && hi <= last.m_energy) return true;
        }
    }
    return false;
}

/// all materials that the given compiled element may perform lookups with.
std::vector<int> materialsOf(const OpticalElement& element) {
    std::vector<int> mats;

    // only behaveMirror and behaveFoil call getRefractiveIndex; every other behaviour
    // ignores the material entirely
    if (!element.m_behaviour.is<Behaviour::Mirror>() && !element.m_behaviour.is<Behaviour::Foil>()) return mats;

    if (element.m_material >= 1 && element.m_material <= 140) mats.push_back(element.m_material);

    element.m_coating.visit([&](const auto& coating) {
        using T = std::decay_t<decltype(coating)>;
        if constexpr (std::is_same_v<T, Coating::OneCoating>) {
            if (coating.material >= 1 && coating.material <= 140) mats.push_back(coating.material);
        } else if constexpr (std::is_same_v<T, Coating::MultilayerCoating>) {
            for (int i = 0; i < coating.numLayers; ++i) {
                if (coating.material[i] >= 1 && coating.material[i] <= 140) mats.push_back(coating.material[i]);
            }
        }
    });
    return mats;
}

}  // unnamed namespace

void Group::verifyMaterialCoverage(std::vector<OpticalElementAndTransform>& compiledElements, const MaterialTables& tables) const {
    double energyLo = std::numeric_limits<double>::max();
    double energyHi = std::numeric_limits<double>::lowest();
    for (const auto* source : getSources()) {
        // GenRays skips these too
        if (source->getType() == ElementType::RayListSource) continue;
        auto range = energyRangeOf(source->getEnergyDistribution());
        if (!range) continue;
        energyLo = std::min(energyLo, range->first);
        energyHi = std::max(energyHi, range->second);
    }
    if (energyLo > energyHi) return;

    for (size_t i = 0; i < compiledElements.size(); i++) {
        auto& element = compiledElements[i].element;
        bool covered  = true;
        for (const auto material : materialsOf(element)) {
            if (!materialCoversRange(material, tables, energyLo, energyHi)) {
                covered = false;
                break;
            }
        }
        if (covered) continue;

        bool isFoil = element.m_behaviour.is<Behaviour::Foil>();
        RAYX_WARN << "material data for element " << i << " (" << (isFoil ? "foil" : "element") << ") does not cover the source energy range ["
                  << energyLo << ", " << energyHi << "] eV; compiling it with ideal geometric behaviour "
                  << (isFoil ? "(100% transmission)" : "(100% reflection)") << " instead";

        element.m_material = static_cast<int>(Material::REFLECTIVE);
        element.m_coating  = Coating::SubstrateOnly{};
    }
}

std::vector<const DesignElement*> Group::getElements() const {
    std::vector<const DesignElement*> elements;
    ctraverse([&elements](const BeamlineNode& node) -> bool {
        if (node.isElement()) { elements.push_back(static_cast<const DesignElement*>(&node)); }
        return false;
    });
    return elements;
}

std::vector<const DesignSource*> Group::getSources() const {
    std::vector<const DesignSource*> sources;
    ctraverse([&sources](const BeamlineNode& node) -> bool {
        if (node.isSource()) { sources.push_back(static_cast<const DesignSource*>(&node)); }
        return false;
    });
    return sources;
}

std::vector<std::string> Group::getElementNames() const {
    std::vector<std::string> names;
    ctraverse([&names](const BeamlineNode& node) -> bool {
        if (node.isElement()) {
            const auto* element = static_cast<const DesignElement*>(&node);
            names.push_back(element->getName());
        }
        return false;
    });
    return names;
}

std::vector<std::string> Group::getSourceNames() const {
    std::vector<std::string> names;
    ctraverse([&names](const BeamlineNode& node) -> bool {
        if (node.isSource()) {
            const auto* source = static_cast<const DesignSource*>(&node);
            names.push_back(source->getName());
        }
        return false;
    });
    return names;
}

std::vector<std::string> Group::getObjectNames() const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    std::vector<std::string> names;

    const auto size = numObjects();
    for (size_t i = 0; i < size; ++i) {
        const auto* node = findNodeByObjectId(i);
        names.push_back(node->getName());
    }

    return names;
}

size_t Group::numElements() const {
    size_t count = 0;
    ctraverse([&count](const BeamlineNode& node) -> bool {
        if (node.isElement()) { ++count; }
        return false;
    });
    return count;
}

size_t Group::numSources() const {
    size_t count = 0;
    ctraverse([&count](const BeamlineNode& node) -> bool {
        if (node.isSource()) { ++count; }
        return false;
    });
    return count;
}

size_t Group::numObjects() const {
    size_t count = 0;
    ctraverse([&count](const BeamlineNode& node) -> bool {
        if (node.isSource() || node.isElement()) { ++count; }
        return false;
    });
    return count;
}

size_t Group::numRayPaths() const {
    size_t count = 0;
    ctraverse([&count](const BeamlineNode& node) -> bool {
        if (node.isSource()) {
            const auto* source = static_cast<const DesignSource*>(&node);
            count += source->getNumberOfRays();
        }
        return false;
    });
    return count;
}

size_t Group::findObjectIdByNode(const BeamlineNode* node) const {
    if (m_parent) throw std::runtime_error("BeamlineNode::getObjectId(BeamlineNode* node) may only be called on the root node");
    if (node->isGroup()) throw std::runtime_error("error: BeamlineNode::getObjectId(BeamlineNode* node) cannot be called with a Group node");

    if (node->isSource()) {
        size_t count = 0;
        ctraverse([&](const BeamlineNode& node_) -> bool {
            if (node == &node_) return true;
            if (node_.isSource()) ++count;
            return false;
        });
        return count;
    }

    if (node->isElement()) {
        size_t count = 0;
        ctraverse([&](const BeamlineNode& node_) -> bool {
            if (node == &node_) return true;
            if (node_.isElement()) ++count;
            return false;
        });
        return count + numSources();
    }

    RAYX_EXIT << "error: unimpemented BeamlineNode type";
    return 0;
}

const BeamlineNode* Group::findNodeByObjectId(const size_t objectId) const {
    const auto root = getRoot();
    if (root != this) RAYX_EXIT << "error: BeamlineNode::getBeamlineNode(size_t objectId) must only be called on the root node";

    if (objectId >= numObjects())
        throw std::out_of_range(std::format("Group::findNodeByObjectId: objectId {} is out of bounds [0, {})", objectId, numObjects()));

    const BeamlineNode* node;
    const auto numberOfSources = numSources();

    if (objectId < numberOfSources) {
        size_t count = 0;
        ctraverse([&](const BeamlineNode& node_) -> bool {
            if (node_.isSource()) {
                if (objectId == count) {
                    node = &node_;
                    return true;
                }
                ++count;
            }
            return false;
        });
    } else {
        size_t count = 0;
        ctraverse([&](const BeamlineNode& node_) -> bool {
            if (node_.isElement()) {
                if (objectId - numberOfSources == count) {
                    node = &node_;
                    return true;
                }
                ++count;
            }
            return false;
        });
    }

    if (!node)
        RAYX_EXIT << "error: could not find BeamlineNode with object_id = " << objectId
                  << ". This is an unknown error detected during call of BeamlineNode::getBeamlineNode(size_t objectId)";
    return node;
}

BeamlineNode* Group::findNodeByObjectId(const size_t objectId) {
    auto constSelf = const_cast<const Group*>(this);
    return const_cast<BeamlineNode*>(constSelf->findNodeByObjectId(objectId));
}

}  // namespace rayx
