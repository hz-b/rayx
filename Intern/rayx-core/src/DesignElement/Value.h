#pragma once

#include <unordered_map>
#include <variant>

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
    CentralBeamStop,
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
    SigmaType
};

class Undefined {};

class Value;
using Map = std::unordered_map<std::string, Value>;

class Value {
  public:
    Value() : m_variant(Undefined()) {}
    Value(double x) : m_variant(x) {}
    Value(int x) : m_variant(x) {}
    Value(bool x) : m_variant(x) {}
    Value(std::string x) : m_variant(x) {}
    Value(Map x) : m_variant(x) {}
    Value(dvec4 x) : m_variant(x) {}
    Value(glm::dmat4x4 x) : m_variant(x) {}
    Value(Rad x) : m_variant(x) {}
    Value(Material x) : m_variant(x) {}
    Value(Misalignment x) : m_variant(x) {}
    Value(CentralBeamstop x) : m_variant(x) {}
    Value(Cutout x) : m_variant(x) {}
    Value(CylinderDirection x) : m_variant(x) {}
    Value(FigureRotation x) : m_variant(x) {}
    Value(CurvatureType x) : m_variant(x) {}
    Value(Surface x) : m_variant(x) {}
    Value(SourceDist x) : m_variant(x) {}
    Value(SpreadType x) : m_variant(x) {}
    Value(EnergyDistributionType x) : m_variant(x) {}
    Value(EnergySpreadUnit x) : m_variant(x) {}
    Value(SigmaType x) : m_variant(x) {}

    void operator=(double x) { m_variant = x; }
    void operator=(int x) { m_variant = x; }
    void operator=(bool x) { m_variant = x; }
    void operator=(std::string x) { m_variant = x; }
    void operator=(Map x) { m_variant = x; }
    void operator=(dvec4 x) { m_variant = x; }
    void operator=(glm::dmat4x4 x) { m_variant = x; }
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



    inline ValueType type() const {
        const ValueType types[] = {
            ValueType::Undefined, ValueType::Double,     ValueType::Int,           ValueType::CylinderDirection,
            ValueType::String,    ValueType::Map,        ValueType::Dvec4,         ValueType::EnergySpreadUnit,
            ValueType::Rad,       ValueType::Material,   ValueType::Misalignment,  ValueType::CentralBeamStop,
            ValueType::Cutout,    ValueType::Bool,       ValueType::FigureRotation,ValueType::CurvatureType,
            ValueType::Surface,   ValueType::SourceDist, ValueType::SpreadType,    ValueType::EnergyDistributionType,
            ValueType::Dmat4x4,   ValueType::SigmaType,  ValueType::ElectronEnergyOrientation           
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

    inline glm::dmat4x4 as_dmat4x4() const {
        auto* x = std::get_if<glm::dmat4x4>(&m_variant);
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

    const Value& operator[](std::string s) const {
        const Map* m = std::get_if<Map>(&m_variant);
        if (!m) throw std::runtime_error("Indexing into non-map!");
        return (*m).at(s);  // TODO return undefined on missing
    }

    Value& operator[](std::string s) {
        Map* m = std::get_if<Map>(&m_variant);
        if (!m) {
            throw std::runtime_error("Indexing into non-map!");
        }
        return (*m)[s];
    }

  private:
    std::variant<
                 Undefined,        double,          int,     ElectronEnergyOrientation,
                 glm::dvec4,       glm::dmat4x4,    bool,    EnergyDistributionType, 
                 Misalignment,     CentralBeamstop, Cutout,  CylinderDirection, 
                 FigureRotation,   Map,             Surface, CurvatureType,
                 SourceDist,       SpreadType,      Rad,     Material,
                 EnergySpreadUnit, std::string,     SigmaType
                > m_variant;
};
}  // namespace RAYX