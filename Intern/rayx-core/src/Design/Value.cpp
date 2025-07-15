#include "Design/Value.h"

#include <stdexcept>

namespace RAYX {

DesignMap DesignMap::clone() const {
    DesignMap copy;
    // If the variant holds a Map, we recursively clone each entry.
    if (std::holds_alternative<Map>(m_variant)) {
        Map newMap;
        const Map& oldMap = std::get<Map>(m_variant);
        for (const auto& [key, ptr] : oldMap) {
            newMap[key] = std::make_shared<DesignMap>(ptr->clone());
        }
        copy.m_variant = newMap;
    } else {
        // For all other types, the variant’s copy is sufficient.
        copy.m_variant = m_variant;
    }
    return copy;
}

ValueType DesignMap::type() const {
    const ValueType types[] = {
        ValueType::Undefined,
        ValueType::Double,
        ValueType::Int,
        ValueType::ElectronEnergyOrientation,
        ValueType::Dvec4,
        ValueType::Dmat4x4,
        ValueType::Bool,
        ValueType::EnergyDistributionType,
        ValueType::Misalignment,
        ValueType::CentralBeamstop,
        ValueType::Cutout,
        ValueType::CutoutType,  // Added
        ValueType::EventType,   // Added
        ValueType::CylinderDirection,
        ValueType::FigureRotation,
        ValueType::Map,
        ValueType::Surface,
        ValueType::CurvatureType,
        ValueType::SourceDist,
        ValueType::SpreadType,
        ValueType::Rad,
        ValueType::Material,
        ValueType::EnergySpreadUnit,
        ValueType::String,
        ValueType::SigmaType,
        ValueType::BehaviourType,
        ValueType::ElementType,
        ValueType::GratingMount,
        ValueType::CrystalType,
    };
    return types[m_variant.index()];
}

double DesignMap::as_double() const {
    if (auto* x = std::get_if<double>(&m_variant)) return *x;
    throw std::runtime_error("as_double() called on non-double!");
}

int DesignMap::as_int() const {
    if (auto* x = std::get_if<int>(&m_variant)) return *x;
    throw std::runtime_error("as_int() called on non-int!");
}

bool DesignMap::as_bool() const {
    if (auto* x = std::get_if<bool>(&m_variant)) return *x;
    throw std::runtime_error("as_bool() called on non-bool!");
}

std::string DesignMap::as_string() const {
    if (auto* x = std::get_if<std::string>(&m_variant)) return *x;
    throw std::runtime_error("as_string() called on non-string!");
}

Map DesignMap::as_map() const {
    if (auto* x = std::get_if<Map>(&m_variant)) return *x;
    throw std::runtime_error("as_map() called on non-map!");
}

glm::dvec4 DesignMap::as_dvec4() const {
    if (auto* x = std::get_if<glm::dvec4>(&m_variant)) return *x;
    throw std::runtime_error("as_dvec4() called on non-dvec4!");
}

glm::dmat4 DesignMap::as_dmat4x4() const {
    if (auto* x = std::get_if<glm::dmat4x4>(&m_variant)) return *x;
    throw std::runtime_error("as_dmat4x4() called on non-dmat4x4!");
}

Rad DesignMap::as_rad() const {
    if (auto* x = std::get_if<Rad>(&m_variant)) return *x;
    throw std::runtime_error("as_rad() called on non-rad!");
}

Material DesignMap::as_material() const {
    if (auto* x = std::get_if<Material>(&m_variant)) return *x;
    throw std::runtime_error("as_material() called on non-material!");
}

Misalignment DesignMap::as_misalignment() const {
    if (auto* x = std::get_if<Misalignment>(&m_variant)) return *x;
    throw std::runtime_error("as_misalignment() called on non-misalignment!");
}

CentralBeamstop DesignMap::as_centralBeamStop() const {
    if (auto* x = std::get_if<CentralBeamstop>(&m_variant)) return *x;
    throw std::runtime_error("as_centralBeamStop() called on non-centralBeamStop!");
}

Cutout DesignMap::as_cutout() const {
    if (auto* x = std::get_if<Cutout>(&m_variant)) return *x;
    throw std::runtime_error("as_cutout() called on non-cutout!");
}

CutoutType DesignMap::as_openingShape() const {
    if (auto* x = std::get_if<CutoutType>(&m_variant)) return *x;
    throw std::runtime_error("as_openingShape() called on non-openingshape!");
}

CylinderDirection DesignMap::as_cylinderDirection() const {
    if (auto* x = std::get_if<CylinderDirection>(&m_variant)) return *x;
    throw std::runtime_error("as_cylinderDirection() called on non-cylinderDirection!");
}

FigureRotation DesignMap::as_figureRotation() const {
    if (auto* x = std::get_if<FigureRotation>(&m_variant)) return *x;
    throw std::runtime_error("as_figureRotation() called on non-figureRotation!");
}

CurvatureType DesignMap::as_curvatureType() const {
    if (auto* x = std::get_if<CurvatureType>(&m_variant)) return *x;
    throw std::runtime_error("as_curvatureType() called on non-curvatureType!");
}

Surface DesignMap::as_surface() const {
    if (auto* x = std::get_if<Surface>(&m_variant)) return *x;
    throw std::runtime_error("as_surface() called on non-surface!");
}

SourceDist DesignMap::as_sourceDist() const {
    if (auto* x = std::get_if<SourceDist>(&m_variant)) return *x;
    throw std::runtime_error("as_sourceDist() called on non-sourceDist!");
}

SpreadType DesignMap::as_energySpreadType() const {
    if (auto* x = std::get_if<SpreadType>(&m_variant)) return *x;
    throw std::runtime_error("as_energySpreadType() called on non-energySpreadType!");
}

EnergyDistributionType DesignMap::as_energyDistType() const {
    if (auto* x = std::get_if<EnergyDistributionType>(&m_variant)) return *x;
    throw std::runtime_error("as_energyDistType() called on non-energyDistType!");
}

EnergySpreadUnit DesignMap::as_energySpreadUnit() const {
    if (auto* x = std::get_if<EnergySpreadUnit>(&m_variant)) return *x;
    throw std::runtime_error("as_energySpreadUnit() called on non-energySpreadUnit!");
}

ElectronEnergyOrientation DesignMap::as_electronEnergyOrientation() const {
    if (auto* x = std::get_if<ElectronEnergyOrientation>(&m_variant)) return *x;
    throw std::runtime_error("as_electronEnergyOrientation() called on non-electronEnergyOrientation!");
}

SigmaType DesignMap::as_sigmaType() const {
    if (auto* x = std::get_if<SigmaType>(&m_variant)) return *x;
    throw std::runtime_error("as_sigmaType() called on non-sigmaType!");
}

BehaviourType DesignMap::as_behaviourType() const {
    if (auto* x = std::get_if<BehaviourType>(&m_variant)) return *x;
    throw std::runtime_error("as_behaviourType() called on non-behaviourType!");
}

ElementType DesignMap::as_elementType() const {
    if (auto* x = std::get_if<ElementType>(&m_variant)) return *x;
    throw std::runtime_error("as_elementType() called on non-elementType!");
}

GratingMount DesignMap::as_gratingMount() const {
    if (auto* x = std::get_if<GratingMount>(&m_variant)) return *x;
    throw std::runtime_error("as_gratingMount() called on non-gratingMount!");
}

CrystalType DesignMap::as_crystalType() const {
    if (auto* x = std::get_if<CrystalType>(&m_variant)) return *x;
    throw std::runtime_error("as_crystalType() called on non-crystalType!");
}

const DesignMap& DesignMap::operator[](const std::string& s) const {
    if (auto* m = std::get_if<Map>(&m_variant)) {
        auto it = m->find(s);
        if (it == m->end()) {
            throw std::runtime_error("Indexing into non-map at: " + s);
        }
        return *(it->second);
    }
    throw std::runtime_error("Indexing into non-map at: " + s);
}

DesignMap& DesignMap::operator[](const std::string& s) {
    if (auto* m = std::get_if<Map>(&m_variant)) {
        if (m->find(s) == m->end()) {
            (*m)[s] = std::make_shared<DesignMap>();
        }
        return *((*m)[s]);
    }
    throw std::runtime_error("Indexing into non-map!");
}

DesignMap::Iterator DesignMap::begin() {
    if (auto* m = std::get_if<Map>(&m_variant)) return Iterator(m->begin());
    throw std::runtime_error("Calling begin() on non-map!");
}

DesignMap::Iterator DesignMap::end() {
    if (auto* m = std::get_if<Map>(&m_variant)) return Iterator(m->end());
    throw std::runtime_error("Calling end() on non-map!");
}

DesignMap::ConstIterator DesignMap::begin() const {
    if (auto* m = std::get_if<Map>(&m_variant)) return ConstIterator(m->begin());
    throw std::runtime_error("Calling begin() on non-map!");
}

DesignMap::ConstIterator DesignMap::end() const {
    if (auto* m = std::get_if<Map>(&m_variant)) return ConstIterator(m->end());
    throw std::runtime_error("Calling end() on non-map!");
}

}  // namespace RAYX
