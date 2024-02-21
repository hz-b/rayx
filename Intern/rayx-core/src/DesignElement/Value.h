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
    Cutout
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


    inline ValueType type() const {
        const ValueType types[] = {
            ValueType::Undefined, ValueType::Double,   ValueType::Int,          ValueType::Bool,
            ValueType::String,    ValueType::Map,      ValueType::Dvec4,        ValueType::Dmat4x4,
            ValueType::Rad,       ValueType::Material, ValueType::Misalignment, ValueType::CentralBeamStop,
            ValueType::Cutout,
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

    const Value& operator[](std::string s) const {
        const Map* m = std::get_if<Map>(&m_variant);
        if (!m) throw std::runtime_error("Indexing into non-map!");
        return (*m).at(s);  // TODO return undefined on missing
    }

    Value& operator[](std::string s) {
        Map* m = std::get_if<Map>(&m_variant);
        if (!m) throw std::runtime_error("Indexing into non-map!");
        return (*m)[s];
    }

  private:
    std::variant<Undefined, double, int, std::string, Map, glm::dvec4, glm::dmat4x4, bool, Rad, Material, Misalignment, CentralBeamstop, Cutout> m_variant;
};
}  // namespace RAYX