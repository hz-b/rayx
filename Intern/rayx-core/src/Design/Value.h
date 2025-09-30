#pragma once

#include <iterator>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

// Include your other dependencies.
#include "Angle.h"
#include "Beamline/EnergyDistribution.h"
#include "Core.h"
#include "Debug/Debug.h"
#include "Element/Cutout.h"
#include "Element/Surface.h"
#include "Material/Material.h"
#include "Rml/xml.h"

namespace RAYX {

enum class ValueType {
    Undefined,
    Double,
    Int,
    String,
    Map,
    Dvec4,
    Dmat4x4,
    Bool,
    Rad,
    Material,
    CentralBeamstop,
    Cutout,
    CutoutType,
    EventType,
    CylinderDirection,
    FigureRotation,
    CurvatureType,
    Surface,
    SourceDist,
    SpreadType,
    EnergyDistributionType,
    EnergySpreadUnit,
    ElectronEnergyOrientation,
    SigmaType,
    BehaviourType,
    ElementType,
    GratingMount,
    CrystalType,
    DesignPlane
};

class Undefined {};

class DesignMap;

/**
 * This Map is the foundation for the DesignELement ad DesignSource
 * All Parameter are defined by a string set in DesignElement.cpp and a Value.
 * The Value describes the possible Types. It is defined as a shared pointer because of the recursive call.
 *
 */
using Map = std::unordered_map<std::string, std::shared_ptr<DesignMap>>;

/**
 * To ensure a typesafe Map all possible options are defined in the Value class bellow
 */
class RAYX_API DesignMap {
  public:
    // Constructors
    DesignMap() : m_variant(Undefined()) {}
    DesignMap(double x) : m_variant(x) {}
    DesignMap(int x) : m_variant(x) {}
    DesignMap(bool x) : m_variant(x) {}
    DesignMap(const std::string& x) : m_variant(x) {}
    DesignMap(Map x) : m_variant(x) {}
    DesignMap(glm::dvec4 x) : m_variant(x) {}
    DesignMap(glm::dmat4x4 x) : m_variant(x) {}
    DesignMap(Rad x) : m_variant(x) {}
    DesignMap(Material x) : m_variant(x) {}
    DesignMap(CentralBeamstop x) : m_variant(x) {}
    DesignMap(Cutout x) : m_variant(x) {}
    DesignMap(CutoutType x) : m_variant(x) {}
    DesignMap(EventType x) : m_variant(x) {}
    DesignMap(CylinderDirection x) : m_variant(x) {}
    DesignMap(FigureRotation x) : m_variant(x) {}
    DesignMap(CurvatureType x) : m_variant(x) {}
    DesignMap(Surface x) : m_variant(x) {}
    DesignMap(SourceDist x) : m_variant(x) {}
    DesignMap(SpreadType x) : m_variant(x) {}
    DesignMap(EnergyDistributionType x) : m_variant(x) {}
    DesignMap(EnergySpreadUnit x) : m_variant(x) {}
    DesignMap(SigmaType x) : m_variant(x) {}
    DesignMap(BehaviourType x) : m_variant(x) {}
    DesignMap(ElementType x) : m_variant(x) {}
    DesignMap(GratingMount x) : m_variant(x) {}
    DesignMap(CrystalType x) : m_variant(x) {}
    DesignMap(DesignPlane x) : m_variant(x) {}

    // Assignment operators
    void operator=(double x) { m_variant = x; }
    void operator=(int x) { m_variant = x; }
    void operator=(bool x) { m_variant = x; }
    void operator=(const std::string& x) { m_variant = x; }
    void operator=(Map x) { m_variant = x; }
    void operator=(glm::dvec4 x) { m_variant = x; }
    void operator=(glm::dmat4 x) { m_variant = x; }
    void operator=(Rad x) { m_variant = x; }
    void operator=(Material x) { m_variant = x; }
    void operator=(CentralBeamstop x) { m_variant = x; }
    void operator=(Cutout x) { m_variant = x; }
    void operator=(CutoutType x) { m_variant = x; }
    void operator=(EventType x) { m_variant = x; }
    void operator=(CylinderDirection x) { m_variant = x; }
    void operator=(FigureRotation x) { m_variant = x; }
    void operator=(CurvatureType x) { m_variant = x; }
    void operator=(Surface x) { m_variant = x; }
    void operator=(SourceDist x) { m_variant = x; }
    void operator=(SpreadType x) { m_variant = x; }
    void operator=(EnergyDistributionType x) { m_variant = x; }
    void operator=(EnergySpreadUnit x) { m_variant = x; }
    void operator=(ElectronEnergyOrientation x) { m_variant = x; }
    void operator=(SigmaType x) { m_variant = x; }
    void operator=(BehaviourType x) { m_variant = x; }
    void operator=(GratingMount x) { m_variant = x; }
    void operator=(ElementType x) { m_variant = x; }
    void operator=(CrystalType x) { m_variant = x; }
    void operator=(DesignPlane x) { m_variant = x; }

    // Deep copy (clone) method.
    DesignMap clone() const;

    // Accessor functions.
    ValueType type() const;
    double as_double() const;
    int as_int() const;
    bool as_bool() const;
    std::string as_string() const;
    Map as_map() const;
    glm::dvec4 as_dvec4() const;
    glm::dmat4 as_dmat4x4() const;
    Rad as_rad() const;
    Material as_material() const;
    CentralBeamstop as_centralBeamStop() const;
    Cutout as_cutout() const;
    CutoutType as_openingShape() const;
    EventType as_eventType() const;
    CylinderDirection as_cylinderDirection() const;
    FigureRotation as_figureRotation() const;
    CurvatureType as_curvatureType() const;
    Surface as_surface() const;
    SourceDist as_sourceDist() const;
    SpreadType as_energySpreadType() const;
    EnergyDistributionType as_energyDistType() const;
    EnergySpreadUnit as_energySpreadUnit() const;
    ElectronEnergyOrientation as_electronEnergyOrientation() const;
    SigmaType as_sigmaType() const;
    BehaviourType as_behaviourType() const;
    ElementType as_elementType() const;
    GratingMount as_gratingMount() const;
    CrystalType as_crystalType() const;
    DesignPlane as_designPlane() const;

    // Subscript operators.
    const DesignMap& operator[](const std::string& s) const;
    DesignMap& operator[](const std::string& s);

    EnergyDistributionType as_energyDistributionType() const {
        auto* x = std::get_if<EnergyDistributionType>(&m_variant);
        if (!x) throw std::runtime_error("as_energyDistributionType() called on non-energyDistributionType!");
        return *x;
    }

    // Iterator classes
    // Iterator classes (simple wrappers around the underlying Map iterators).
    class Iterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::pair<const std::string, std::shared_ptr<DesignMap>>;
        using pointer           = value_type*;
        using reference         = value_type&;

        Iterator(Map::iterator it) : m_it(it) {}
        reference operator*() const { return *m_it; }
        pointer operator->() { return &(*m_it); }
        Iterator& operator++() {
            ++m_it;
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        friend bool operator==(const Iterator& a, const Iterator& b) { return a.m_it == b.m_it; }
        friend bool operator!=(const Iterator& a, const Iterator& b) { return a.m_it != b.m_it; }

      private:
        Map::iterator m_it;
    };

    class ConstIterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = const std::pair<const std::string, std::shared_ptr<DesignMap>>;
        using pointer           = const value_type*;
        using reference         = const value_type&;

        ConstIterator(Map::const_iterator it) : m_it(it) {}
        reference operator*() const { return *m_it; }
        pointer operator->() { return &(*m_it); }
        ConstIterator& operator++() {
            ++m_it;
            return *this;
        }
        ConstIterator operator++(int) {
            ConstIterator tmp(*this);
            ++(*this);
            return tmp;
        }
        friend bool operator==(const ConstIterator& a, const ConstIterator& b) { return a.m_it == b.m_it; }
        friend bool operator!=(const ConstIterator& a, const ConstIterator& b) { return a.m_it != b.m_it; }

      private:
        Map::const_iterator m_it;
    };

    // Begin/end for iterators.
    Iterator begin();
    Iterator end();
    ConstIterator begin() const;
    ConstIterator end() const;

  private:
    std::variant<Undefined, double, int, ElectronEnergyOrientation, glm::dvec4, glm::dmat4x4, bool, EnergyDistributionType, CentralBeamstop, Cutout,
                 CutoutType, EventType, CylinderDirection, FigureRotation, Map, Surface, CurvatureType, SourceDist, SpreadType, Rad, Material,
                 EnergySpreadUnit, std::string, SigmaType, BehaviourType, ElementType, GratingMount, CrystalType, DesignPlane>
        m_variant;
};

}  // namespace RAYX
