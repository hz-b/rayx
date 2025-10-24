#pragma once

#include <variant>
#include <vector>

#include "Core.h"
#include "Debug/Debug.h"

namespace rayx {

class RAYX_API ObjectIndexMask {
    ObjectIndexMask(const std::vector<bool>& sourceMask, const std::vector<bool>& elementMask)
        : m_sourceMask(sourceMask), m_elementMask(elementMask) {}
    ObjectIndexMask(std::vector<bool>&& sourceMask, std::vector<bool>&& elementMask)
        : m_sourceMask(std::move(sourceMask)), m_elementMask(std::move(elementMask)) {}

  public:
    ObjectIndexMask()                                  = default;
    ObjectIndexMask(const ObjectIndexMask&)            = default;
    ObjectIndexMask(ObjectIndexMask&&)                 = default;
    ObjectIndexMask& operator=(const ObjectIndexMask&) = default;
    ObjectIndexMask& operator=(ObjectIndexMask&&)      = default;

    static ObjectIndexMask none(const int numSources, const int numElements) {
        return {
            std::vector<bool>(numSources, false),
            std::vector<bool>(numElements, false),
        };
    }

    static ObjectIndexMask all(const int numSources, const int numElements) {
        return {
            std::vector<bool>(numSources, true),
            std::vector<bool>(numElements, true),
        };
    }

    static ObjectIndexMask allSources(const int numSources, const int numElements) {
        return {
            std::vector<bool>(numSources, true),
            std::vector<bool>(numElements, false),
        };
    }

    static ObjectIndexMask allElements(const int numSources, const int numElements) {
        return {
            std::vector<bool>(numSources, false),
            std::vector<bool>(numElements, true),
        };
    }

    static ObjectIndexMask byIndices(const int numSources, const int numElements, const std::vector<int>& objectIndices) {
        auto sourceMask  = std::vector<bool>(numSources, false);
        auto elementMask = std::vector<bool>(numElements, false);

        for (const auto i : objectIndices) {
            if (i < numSources)
                sourceMask.at(i) = true;
            else
                elementMask.at(i - numSources) = true;
        }

        return {
            sourceMask,
            elementMask,
        };
    }

    int numSources() const { return static_cast<int>(m_sourceMask.size()); }
    int numElements() const { return static_cast<int>(m_elementMask.size()); }
    int numObjects() const { return numSources() + numElements(); }

    int numSourcesToRecord() const { return static_cast<int>(std::count(m_sourceMask.begin(), m_sourceMask.end(), true)); }
    int numElementsToRecord() const { return static_cast<int>(std::count(m_elementMask.begin(), m_elementMask.end(), true)); }
    int numObjectsToRecord() const { return numSourcesToRecord() + numElementsToRecord(); }

    bool shouldRecordSource(int source_id) const { return m_sourceMask.at(source_id); }
    void setShouldRecordSource(int source_id, bool value) { m_sourceMask.at(source_id) = value; }

    bool shouldRecordElement(int element_id) const { return m_elementMask.at(element_id); }
    void setShouldRecordElement(int element_id, bool value) { m_elementMask.at(element_id) = value; }

    bool shouldRecordObject(int object_id) const {
        const auto numSources = static_cast<int>(m_sourceMask.size());
        if (object_id < numSources) return m_sourceMask.at(object_id);
        return m_elementMask.at(object_id - numSources);
    }

    void setShouldRecordObject(int object_id, bool value) {
        const auto numSources = static_cast<int>(m_sourceMask.size());
        if (object_id < numSources) m_sourceMask.at(object_id) = value;
        m_elementMask.at(object_id - numSources) = value;
    }

  private:
    std::vector<bool> m_sourceMask;
    std::vector<bool> m_elementMask;
};

class RAYX_API ObjectMask {
    struct None {};
    struct All {};
    struct AllSources {};
    struct AllElements {};
    struct ByIndices {
        std::vector<int> indices;
    };
    using Variant = std::variant<None, All, AllSources, AllElements, ByIndices, ObjectIndexMask>;

    ObjectMask(Variant variant) : m_variant(std::move(variant)) {}

  public:
    ObjectMask() : m_variant(None{}) {}
    ObjectMask(const ObjectMask&)            = default;
    ObjectMask(ObjectMask&&)                 = default;
    ObjectMask& operator=(const ObjectMask&) = default;
    ObjectMask& operator=(ObjectMask&&)      = default;
    ObjectMask(ObjectIndexMask objectIndexMask) : m_variant(std::move(objectIndexMask)) {}

    static ObjectMask none() { return ObjectMask(None{}); }
    static ObjectMask all() { return ObjectMask(All{}); }
    static ObjectMask allSources() { return ObjectMask(AllSources{}); }
    static ObjectMask allElements() { return ObjectMask(AllElements{}); }
    static ObjectMask byIndices(std::vector<int> indices) { return ObjectMask(ByIndices{.indices = std::move(indices)}); }

    // resolve to a concrete mask
    ObjectIndexMask toObjectIndexMask(int numSources, int numElements) const {
        return std::visit(
            [&]<typename T>(const T& option) -> ObjectIndexMask {
                if constexpr (std::is_same_v<T, None>) return ObjectIndexMask::none(numSources, numElements);
                if constexpr (std::is_same_v<T, All>) return ObjectIndexMask::all(numSources, numElements);
                if constexpr (std::is_same_v<T, AllSources>) return ObjectIndexMask::allSources(numSources, numElements);
                if constexpr (std::is_same_v<T, AllElements>) return ObjectIndexMask::allElements(numSources, numElements);
                if constexpr (std::is_same_v<T, ByIndices>) return ObjectIndexMask::byIndices(numSources, numElements, option.indices);
                if constexpr (std::is_same_v<T, ObjectIndexMask>) return option;

                RAYX_EXIT << "error: unknown ObjectMask option: " << typeid(T).name();
                return {};
            },
            m_variant);
    }

  private:
    Variant m_variant;
};

}  // namespace rayx
