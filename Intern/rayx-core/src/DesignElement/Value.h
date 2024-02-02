#include <variant>
#include <unordered_map>

enum class ValueType {
    Undefined,
    Double,
    Int,
    String,
    Map,
};

class Undefined {};

class Value;
using Map = std::unordered_map<std::string, Value>;

class Value {
    public:

    //TODO add dmat4x4, dvec4, std::vector<?> values
    Value() : m_variant(Undefined()) {}
    Value(double x) : m_variant(x) {}
    Value(int x) : m_variant(x) {}
    Value(std::string x) : m_variant(x) {}
    Value(Map x) : m_variant(x) {}

    void operator=(double x) { m_variant = x; }
    void operator=(int x) { m_variant = x; }
    void operator=(std::string x) { m_variant = x; }
    void operator=(Map x) { m_variant = x; }

    inline ValueType type() const {
        const ValueType types[] = {
            ValueType::Undefined, ValueType::Double, ValueType::Int, ValueType::String, ValueType::Map
        };
        return types[m_variant.index()];
    }

    inline double as_double() {
        auto* x = std::get_if<double>(&m_variant);
        if (!x) throw std::runtime_error("as_double() called on non-double!");
        return *x;
    }

    inline int as_int() {
        auto* x = std::get_if<int>(&m_variant);
        if (!x) throw std::runtime_error("as_int() called on non-int!");
        return *x;
    }

    inline std::string as_string() {
        auto* x = std::get_if<std::string>(&m_variant);
        if (!x) throw std::runtime_error("as_string() called on non-string!");
        return *x;
    }

    inline Map as_map() {
        auto* x = std::get_if<Map>(&m_variant);
        if (!x) throw std::runtime_error("as_map() called on non-map!");
        return *x;
    }

    Value& operator[](std::string s) {
        Map *m = std::get_if<Map>(&m_variant);
        if (!m) throw std::runtime_error("Indexing into non-map!");
        return (*m)[s];
    }

    private:
    std::variant<Undefined, double, int, std::string, Map> m_variant;
};
