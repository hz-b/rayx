#pragma once

#include <numeric>
#include <vector>

#include "IO/Instrumentor.h"
#include "Math/ElectricField.h"
#include "RayAttrMask.h"

namespace rayx::trace {

/**
 * @brief A structure representing a collection of rays and their attributes.
 * Each attribute is stored as a vector, allowing for efficient storage and manipulation of multiple rays.
 * The Rays structure supports move semantics for efficient transfers, but disables copy semantics to prevent accidental
 * costly copies. Use the `copy()` method to create an explicit copy when needed.
 * @note Ensure that all attribute vectors are of the same length to maintain data integrity.
 */
struct RAYX_API Rays {
  protected:
    /// Private copy constructor to prevent accidental copies
    Rays(const Rays&) = default;
    /// Private assignment operator to prevent accidental copies
    Rays& operator=(const Rays&) = default;

  public:
    Rays()                  = default;
    Rays(Rays&&)            = default;
    Rays& operator=(Rays&&) = default;

#define X(type, name, flag) std::vector<type> name;

    RAYX_X_MACRO_RAY_ATTR
#undef X

    glm::dvec3 position(const int i) const { return glm::dvec3(position_x[i], position_y[i], position_z[i]); }
    void position(const int i, const glm::dvec3 position) {
        position_x[i] = position.x;
        position_y[i] = position.y;
        position_z[i] = position.z;
    }

    glm::dvec3 direction(const int i) const { return glm::dvec3(direction_x[i], direction_y[i], direction_z[i]); }
    void direction(const int i, const glm::dvec3 direction) {
        direction_x[i] = direction.x;
        direction_y[i] = direction.y;
        direction_z[i] = direction.z;
    }

    math::ElectricField electric_field(const int i) const {
        return math::ElectricField(electric_field_x[i], electric_field_y[i], electric_field_z[i]);
    }
    void electric_field(const int i, const math::ElectricField electric_field) {
        electric_field_x[i] = electric_field.x;
        electric_field_y[i] = electric_field.y;
        electric_field_z[i] = electric_field.z;
    }

    /**
     * @brief Create a copy of the Rays instance.
     * This function is an explicit copy of the Rays instance to avoid accidental costly copies.
     * If you want to return a Rays instance from a function, you should make use of RVO/NRVO optimizations, instead of
     * using this function.
     * @return A new Rays instance that is a copy of the current instance.
     */
    [[nodiscard]] Rays copy() const;

    /**
     * @brief Get a mask indicating which ray attributes are recorded in this Rays instance.
     * @return A RayAttrMask indicating the recorded attributes.
     */
    RayAttrMask attrMask() const;

    /**
     * @brief Check if a specific attribute is recorded in this Rays instance.
     * @param attr The RayAttrMask attribute to check.
     * @return True if the attribute is recorded, false otherwise.
     * @note When repeatedly checking on the same Rays instance, the free function contains(haystack, needle) should be
     * preferred over this method.
     */
    bool contains(const RayAttrMask attr) const;

    /**
     * @brief Check if the ray list is empty.
     * @return True if the ray list is empty, false otherwise.
     */
    bool empty() const;

    /**
     * @brief Get the number of events in the ray list.
     * @return The number of events in the ray list.
     */
    int size() const;

    /**
     * @brief Get the number of unique paths in the ray list.
     * @return The number of unique path IDs in the ray list.
     * @note Requires that path_id is recorded.
     */
    int numPaths() const;

    /**
     * @brief Append another Rays instance to this one.
     * @param other The Rays instance to append.
     * @return A reference to this Rays instance after appending.
     */
    Rays& append(const Rays& other);

    /**
     * @brief Concatenate multiple Rays instances into a single Rays instance.
     * This is more efficient than using repeated append() calls.
     * @param rays_list A vector of Rays instances to concatenate.
     * @return A new Rays instance containing all rays from the input instances.
     * @note Requires that all Rays instances in rays_list have the same attributes recorded.
     */
    [[nodiscard]] static Rays concat(const std::vector<Rays>& rays_list);

    /**
     * @brief Sort rays by object_id, so that rays interacting with the same object are grouped together.
     * @return A new Rays instance with rays sorted by object_id.
     * @note Requires that object_id is recorded.
     */
    [[nodiscard]] Rays sortByObjectId() const;

    /**
     * @brief Sort rays by path_id and then by path_event_id, so that rays belonging to the same path are grouped
     * together, and within each path, rays are ordered by their event sequence.
     * @return A new Rays instance with rays sorted by path_id and path_event_id.
     * @note Requires that path_id and path_event_id are recorded.
     * @note Every event is uniquely identified by the combination of path_id and path_event_id. Thus, sorting by these
     * two attributes ensures that rays are in a well-defined order. This enables equality comparisons between different
     * Rays instances.
     */
    [[nodiscard]] Rays sortByPathIdAndPathEventId() const;

    /**
     * @brief Sort rays using a custom comparison function.
     * The comparison function should take two indices (int) and return true if the first index should come before the
     * second. This method can be used to implement custom sorting logic, such as sorting by multiple attributes.
     * @tparam Compare A callable type that defines the comparison function.
     * @param comp The comparison function to use for sorting. Must satisfy the requirements of Compare, see
     * https://en.cppreference.com/w/cpp/named_req/Compare. This is the same as for std::sort.
     * @return A new Rays instance with rays sorted according to the comparison function.
     * @note Requires that the attribute to sort by is recorded.
     * @example
     * ```cpp
     * // sort by path_id.
     * rays = rays.sort([&](int lhs, int rhs) { return rays.path_id[lhs] < rays.path_id[rhs]; });
     * // sort by path_id and then by path_event_id.
     * rays = rays.sort([&](int lhs, int rhs) { if (rays.path_id[lhs] == rays.path_id[rhs]) return rays.path_id[lhs] <
     * rays.path_id[rhs]; else return rays.path_event_id[lhs] < rays.path_event_id[rhs]; });
     * ```
     */
    template <typename Compare>
    [[nodiscard]] Rays sort(Compare comp) const;

    /**
     * @brief Filter the rays to only include those with attributes specified in the given mask.
     * This operation modifies the current Rays instance in place.
     * @param mask A RayAttrMask specifying which attributes to retain.
     */
    Rays& filterByAttrMask(const RayAttrMask mask);

    /**
     * @brief Filter the rays to only include those that interacted with a specific object.
     * @param object_id The ID of the object to filter by.
     * @return A new Rays instance containing only rays that interacted with the specified object.
     * @note Requires that object_id is recorded.
     */
    [[nodiscard]] Rays filterByObjectId(const int object_id) const;

    /**
     * @brief Filter the rays to only include the final event of each unique path.
     * The final event is determined by the maximum path_event_id for each path_id.
     * @return A new Rays instance containing only the final event of each path.
     * @note Requires that path_id and path_event_id are recorded.
     */
    [[nodiscard]] Rays filterByLastEventInPath() const;

    /**
     * @brief Filter the rays using a custom predicate function.
     * The predicate function should take an index (int) and return true if the ray at that index should be included.
     * This method can be used to implement custom filtering logic, such as filtering by multiple attributes or complex
     * conditions.
     * @tparam Pred A callable type that defines the predicate function.
     * @param pred The predicate function to use for filtering.
     * @return A new Rays instance containing only rays for which the predicate returns true.
     * @note Requires that path_event_id is recorded.
     * @example
     * ```cpp
     * rays = rays.filter([&](int i) { return rays.path_event_id[i] == 3; }); // to filter by path_event_id == 3.
     * ```
     */
    template <typename Pred>
    [[nodiscard]] Rays filter(Pred pred) const;

    /**
     * @brief Count the number of rays that satisfy a given predicate function.
     * The predicate function should take an index (int) and return true if the ray at that index satisfies the
     * condition.
     * @tparam Pred A callable type that defines the predicate function.
     * @param pred The predicate function to use for counting.
     * @return The number of rays for which the predicate returns true.
     * @example
     * ```cpp
     * int count = rays.count([&](int i) { return rays.object_id[i] == 3; }); // to count rays with object_id == 3.
     * ```
     */
    template <typename Pred>
    int count(Pred pred) const;

    /**
     * @brief Check if the sizes of all recorded attribute vectors are valid (i.e., all the same length).
     * @return True if all recorded attribute vectors have the same length, false otherwise.
     */
    bool isValid() const;

    // TODO: implement helper methods to iterate over attributes, to get rid of most of the X-macros
};

template <typename Compare>
Rays Rays::sort(Compare comp) const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    const auto attr = attrMask();
    const auto n    = size();

    auto indices = std::vector<int>(n);
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), comp);

    Rays result;
#define X(type, name, flag)                                            \
    if (!!(attr & RayAttrMask::flag)) {                                \
        result.name.resize(name.size());                               \
        for (int i = 0; i < n; ++i) result.name[i] = name[indices[i]]; \
    }
    RAYX_X_MACRO_RAY_ATTR
#undef X

    return result;
}

template <typename Pred>
Rays Rays::filter(Pred pred) const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    const auto attr = attrMask();
    const auto n    = size();

    auto indices = std::vector<int>{};
    for (int i = 0; i < n; ++i)
        if (pred(i)) indices.push_back(i);

    Rays result;
#define X(type, name, flag)                                                                                           \
    if (!!(attr & RayAttrMask::flag)) {                                                                               \
        result.name.resize(indices.size());                                                                           \
        std::transform(indices.begin(), indices.end(), result.name.begin(), [this](const int i) { return name[i]; }); \
    }
    RAYX_X_MACRO_RAY_ATTR
#undef X

    return result;
}

template <typename Pred>
int Rays::count(Pred pred) const {
    const int sz = size();
    int count    = 0;
    for (int i = 0; i < sz; ++i)
        if (pred(i)) ++count;
    return count;
}

static_assert(std::is_nothrow_move_constructible_v<Rays>);  // ensure efficient moves, when used in std::vector<Rays>

bool RAYX_API operator==(const Rays& lhs, const Rays& rhs);
bool RAYX_API operator!=(const Rays& lhs, const Rays& rhs);

}  // namespace rayx::trace
