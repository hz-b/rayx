#pragma once

#include <angle.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

#include "Beamline/EnergyDistribution.h"
#include "Beamline/Cutout.h"
#include "Beamline/LightSource.h"
#include "Beamline/Surface.h"
#include "Core.h"
#include "Debug/Debug.h"
#include "Material/Material.h"

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
    Misalignment,
    CentralBeamstop,
    Cutout,
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
    GratingMount
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
class DesignMap {
  public:
    DesignMap() : m_variant(Undefined()) {}
    DesignMap(double x) : m_variant(x) {}
    DesignMap(int x) : m_variant(x) {}
    DesignMap(bool x) : m_variant(x) {}
    DesignMap(std::string x) : m_variant(x) {}
    DesignMap(Map x) : m_variant(x) {}
    DesignMap(glm::dvec4 x) : m_variant(x) {}
    DesignMap(glm::dmat4x4 x) : m_variant(x) {}
    DesignMap(Rad x) : m_variant(x) {}
    DesignMap(Material x) : m_variant(x) {}
    DesignMap(Misalignment x) : m_variant(x) {}
    DesignMap(CentralBeamstop x) : m_variant(x) {}
    DesignMap(Cutout x) : m_variant(x) {}
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

    void operator=(double x) { m_variant = x; }
    void operator=(int x) { m_variant = x; }
    void operator=(bool x) { m_variant = x; }
    void operator=(std::string x) { m_variant = x; }
    void operator=(Map x) { m_variant = x; }
    void operator=(glm::dvec4 x) { m_variant = x; }
    void operator=(glm::dmat4 x) { m_variant = x; }
    void operator=(Rad x) { m_variant = x; }
    void operator=(Material x) { m_variant = x; }
    void operator=(Misalignment x) { m_variant = x; }
    void operator=(CentralBeamstop x) { m_variant = x; }
    void operator=(Cutout x) { m_variant = x; }
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

    inline ValueType type() const {
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
        };
        return types[m_variant.index()];
    }

    inline double as_double() const {
        auto* x = std::get_if<double>(&m_variant);
        if (!x) throw std::runtime_error("as_double() called on non-double!");
        return *x;
    }

    inline int as_int() const {
        auto* x = std::get_if<int>(&m_variant);
        if (!x) throw std::runtime_error("as_int() called on non-int!");
        return *x;
    }

    inline bool as_bool() const {
        auto* x = std::get_if<bool>(&m_variant);
        if (!x) throw std::runtime_error("as_bool() called on non-bool!");
        return *x;
    }

    inline std::string as_string() const {
        auto* x = std::get_if<std::string>(&m_variant);
        if (!x) throw std::runtime_error("as_string() called on non-string!");
        return *x;
    }

    inline Map as_map() const {
        auto* x = std::get_if<Map>(&m_variant);
        if (!x) throw std::runtime_error("as_map() called on non-map!");
        return *x;
    }

    inline glm::dvec4 as_dvec4() const {
        auto* x = std::get_if<glm::dvec4>(&m_variant);
        if (!x) throw std::runtime_error("as_dvec4() called on non-dvec4!");
        return *x;
    }

    inline glm::dmat4 as_dmat4x4() const {
        auto* x = std::get_if<glm::dmat4>(&m_variant);
        if (!x) throw std::runtime_error("as_dmat4x4() called on non-dmat4x4!");
        return *x;
    }

    inline Rad as_rad() const {
        auto* x = std::get_if<Rad>(&m_variant);
        if (!x) throw std::runtime_error("as_rad() called on non-rad!");
        return *x;
    }

    inline Material as_material() const {
        auto* x = std::get_if<Material>(&m_variant);
        if (!x) throw std::runtime_error("as_material() called on non-material!");
        return *x;
    }

    inline Misalignment as_misalignment() const {
        auto* x = std::get_if<Misalignment>(&m_variant);
        if (!x) throw std::runtime_error("as_misalignment() called on non-misalignment!");
        return *x;
    }

    inline CentralBeamstop as_centralBeamStop() const {
        auto* x = std::get_if<CentralBeamstop>(&m_variant);
        if (!x) throw std::runtime_error("as_centralBeamStop() called on non-centralBeamStop!");
        return *x;
    }

    inline Cutout as_cutout() const {
        auto* x = std::get_if<Cutout>(&m_variant);
        if (!x) throw std::runtime_error("as_cutout() called on non-cutout!");
        return *x;
    }

    inline CylinderDirection as_cylinderDirection() const {
        auto* x = std::get_if<CylinderDirection>(&m_variant);
        if (!x) throw std::runtime_error("as_cylinderDirection() called on non-cylinderDirection!");
        return *x;
    }

    inline FigureRotation as_figureRotation() const {
        auto* x = std::get_if<FigureRotation>(&m_variant);
        if (!x) throw std::runtime_error("as_figureRotation() called on non-figureRotation!");
        return *x;
    }

    inline CurvatureType as_curvatureType() const {
        auto* x = std::get_if<CurvatureType>(&m_variant);
        if (!x) throw std::runtime_error("as_curvatureType() called on non-curvatureType!");
        return *x;
    }

    inline Surface as_surface() const {
        auto* x = std::get_if<Surface>(&m_variant);
        if (!x) throw std::runtime_error("as_surface() called on non-surface!");
        return *x;
    }

    inline SourceDist as_sourceDist() const {
        auto* x = std::get_if<SourceDist>(&m_variant);
        if (!x) throw std::runtime_error("as_sourceDist() called on non-sourceDist!");
        return *x;
    }

    inline SpreadType as_energySpreadType() const {
        auto* x = std::get_if<SpreadType>(&m_variant);
        if (!x) throw std::runtime_error("as_energySpreadType() called on non-energySpreadType!");
        return *x;
    }

    inline EnergyDistributionType as_energyDistType() const {
        auto* x = std::get_if<EnergyDistributionType>(&m_variant);
        if (!x) throw std::runtime_error("as_energyDistType() called on non-energyDistType!");
        return *x;
    }

    inline EnergySpreadUnit as_energySpreadUnit() const {
        auto* x = std::get_if<EnergySpreadUnit>(&m_variant);
        if (!x) throw std::runtime_error("as_energySpreadUnit() called on non-energySpreadUnit!");
        return *x;
    }

    inline ElectronEnergyOrientation as_electronEnergyOrientation() const {
        auto* x = std::get_if<ElectronEnergyOrientation>(&m_variant);
        if (!x) throw std::runtime_error("as_electronEnergyOrientation() called on non-electronEnergyOrientation!");
        return *x;
    }

    inline SigmaType as_sigmaType() const {
        auto* x = std::get_if<SigmaType>(&m_variant);
        if (!x) throw std::runtime_error("as_sigmaType() called on non-sigmaType!");
        return *x;
    }

    inline BehaviourType as_behaviourType() const {
        auto* x = std::get_if<BehaviourType>(&m_variant);
        if (!x) throw std::runtime_error("as_behaviourType() called on non-behaviourType!");
        return *x;
    }

    inline ElementType as_elementType() const {
        auto* x = std::get_if<ElementType>(&m_variant);
        if (!x) throw std::runtime_error("as_elementType() called on non-elementType!");
        return *x;
    }

    inline GratingMount as_gratingMount() const {
        auto* x = std::get_if<GratingMount>(&m_variant);
        if (!x) throw std::runtime_error("as_gratingMount() called on non-gratingMount!");
        return *x;
    }

    const DesignMap& operator[](std::string s) const {
        const Map* m = std::get_if<Map>(&m_variant);
        if (!m) throw std::runtime_error("Indexing into non-map at: " + s);
        try {
            auto x = *m->at(s).get();
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            RAYX_LOG << "Indexing into non-map at: " << s;
        }

        return *m->at(s).get();
    }

    DesignMap& operator[](std::string s) {
        Map* m = std::get_if<Map>(&m_variant);
        if (!m) {
            throw std::runtime_error("Indexing into non-map!");
        }

        if (!m->contains(s)) {
            (*m)[s] = std::make_shared<DesignMap>();
        }
        return *((*m)[s].get());
    }
    // Iterator classes
    class Iterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<const std::string, std::shared_ptr<DesignMap>>;
        using pointer = value_type*;
        using reference = value_type&;

        Iterator(Map::iterator it) : m_it(it) {}

        reference operator*() const { return *m_it; }
        pointer operator->() { return &(*m_it); }

        // Prefix increment
        Iterator& operator++() {
            ++m_it;
            return *this;
        }

        // Postfix increment
        Iterator operator++(int) {
            Iterator tmp = *this;
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
        using difference_type = std::ptrdiff_t;
        using value_type = const std::pair<const std::string, std::shared_ptr<DesignMap>>;
        using pointer = const value_type*;
        using reference = const value_type&;

        ConstIterator(Map::const_iterator it) : m_it(it) {}

        reference operator*() const { return *m_it; }
        pointer operator->() { return &(*m_it); }

        // Prefix increment
        ConstIterator& operator++() {
            ++m_it;
            return *this;
        }

        // Postfix increment
        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const ConstIterator& a, const ConstIterator& b) { return a.m_it == b.m_it; }
        friend bool operator!=(const ConstIterator& a, const ConstIterator& b) { return a.m_it != b.m_it; }

      private:
        Map::const_iterator m_it;
    };

    // Begin and end functions for the iterators
    Iterator begin() {
        Map* m = std::get_if<Map>(&m_variant);
        if (!m) throw std::runtime_error("Calling begin() on non-map!");
        return Iterator(m->begin());
    }

    Iterator end() {
        Map* m = std::get_if<Map>(&m_variant);
        if (!m) throw std::runtime_error("Calling end() on non-map!");
        return Iterator(m->end());
    }

    ConstIterator begin() const {
        const Map* m = std::get_if<Map>(&m_variant);
        if (!m) throw std::runtime_error("Calling begin() on non-map!");
        return ConstIterator(m->begin());
    }

    ConstIterator end() const {
        const Map* m = std::get_if<Map>(&m_variant);
        if (!m) throw std::runtime_error("Calling end() on non-map!");
        return ConstIterator(m->end());
    }

  private:
    std::variant<Undefined, double, int, ElectronEnergyOrientation, glm::dvec4, glm::dmat4x4, bool, EnergyDistributionType, Misalignment,
                 CentralBeamstop, Cutout, CylinderDirection, FigureRotation, Map, Surface, CurvatureType, SourceDist, SpreadType, Rad, Material,
                 EnergySpreadUnit, std::string, SigmaType, BehaviourType, ElementType, GratingMount>
        m_variant;
};
}  // namespace RAYX
