#include "Rays.h"

#include <algorithm>
#include <bitset>
#include <unordered_map>

#include "Debug/Instrumentor.h"

namespace RAYX {

Rays Rays::copy() const { return *this; }

RayAttrMask Rays::attrMask() const {
    RayAttrMask mask = RayAttrMask::None;
#define X(type, name, flag) \
    if (name.size() != 0) mask |= RayAttrMask::flag;
    RAYX_X_MACRO_RAY_ATTR
#undef X
    return mask;
}

bool Rays::contains(const RayAttrMask attr) const { return (attrMask() & attr) == attr; }

bool Rays::empty() const { return size() == 0; }

int Rays::size() const {
#define X(type, name, flag) \
    if (name.size() != 0) return static_cast<int>(name.size());
    RAYX_X_MACRO_RAY_ATTR
#undef X
    return 0;
}

int Rays::numPaths() const {
    auto path_ids = path_id;
    std::sort(path_ids.begin(), path_ids.end());
    return std::unique(path_ids.begin(), path_ids.end()) - path_ids.begin();
}

Rays& Rays::append(const Rays& other) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    const auto attr1 = attrMask();
    const auto attr2 = other.attrMask();
    if (attr1 != attr2) throw std::runtime_error("Rays::append requires both Rays to have the same attributes");

#define X(type, name, flag) \
    if (!!(attr1 & RayAttrMask::flag)) name.insert(name.end(), other.name.begin(), other.name.end());
    RAYX_X_MACRO_RAY_ATTR
#undef X

    return *this;
}

Rays Rays::concat(const std::vector<Rays>& rays_list) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    for (size_t i = 1; i < rays_list.size(); ++i) {
        const auto attr1 = rays_list[i - 1].attrMask();
        const auto attr2 = rays_list[i].attrMask();
        if (attr1 != attr2) throw std::runtime_error("Rays::concat requires all Rays to have the same attributes");
    }

    auto n = 0;
    for (const auto& rays : rays_list) n += rays.size();

    Rays result;

    auto offset = 0;
    for (const auto& r : rays_list) {
#define X(type, name, flag)                                                    \
    if (!!(r.attrMask() & RayAttrMask::flag)) {                                \
        result.name.resize(n);                                                 \
        std::copy(r.name.begin(), r.name.end(), result.name.begin() + offset); \
    }
        RAYX_X_MACRO_RAY_ATTR
#undef X
        offset += static_cast<int>(r.size());
    }

    return result;
}

Rays Rays::sortByObjectId() const {
    if (!contains(RayAttrMask::ObjectId)) throw std::runtime_error("Rays::sortByObjectId() requires object_id attribute");
    return sort([&object_id = object_id](int lhs, int rhs) { return object_id[lhs] < object_id[rhs]; });
}

Rays Rays::sortByPathIdAndPathEventId() const {
    if (!contains(RayAttrMask::PathId) || !contains(RayAttrMask::PathEventId))
        throw std::runtime_error("Rays::sortByPathIdAndThenPathEventId() requires path_id and path_event_id attributes.");
    return sort([&path_id = path_id, &path_event_id = path_event_id](int lhs, int rhs) {
        if (path_id[lhs] != path_id[rhs])
            return path_id[lhs] < path_id[rhs];
        else
            return path_event_id[lhs] < path_event_id[rhs];
    });
}

Rays& Rays::filterByAttrMask(const RayAttrMask mask) {
#define X(type, name, flag) \
    if (!(mask & RayAttrMask::flag)) name.clear();
    RAYX_X_MACRO_RAY_ATTR
#undef X
    return *this;
}

Rays Rays::filterByObjectId(const int object_id) const {
    if (!contains(RayAttrMask::ObjectId)) throw std::runtime_error("Rays::filterByObjectId requires object_id attribute");
    return filter([&](int i) { return this->object_id[i] == object_id; });
}

// TODO: revisit this algorithm
Rays Rays::filterByLastEventInPath() const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    if (!contains(RayAttrMask::PathId) || !contains(RayAttrMask::PathEventId))
        throw std::runtime_error("Rays::finalEventsPerPath requires path_id and path_event_id attributes.");

    const auto n = size();
    std::unordered_map<int32_t, int> bestIndex;  // path_id -> index of max path_event_id

    for (int i = 0; i < n; ++i) {
        auto pid   = path_id[i];
        auto pevid = path_event_id[i];
        auto it    = bestIndex.find(pid);
        if (it == bestIndex.end() || pevid > path_event_id[it->second]) { bestIndex[pid] = i; }
    }

    // collect the selected indices
    std::vector<int> indices;
    indices.reserve(bestIndex.size());
    for (auto& kv : bestIndex) { indices.push_back(kv.second); }

    // build result Rays using selected indices
    const auto attr = attrMask();
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

bool Rays::attrSizesAreValid() const {
    const auto attr = attrMask();
    const auto sz   = size();

#define X(type, name, flag) \
    if (RAYX::contains(attr, RayAttrMask::flag) && static_cast<int>(name.size()) != sz) return false;
    RAYX_X_MACRO_RAY_ATTR
#undef X
    return true;
}

bool operator==(const Rays& lhs, const Rays& rhs) {
#define X(type, name, flag) \
    if (lhs.name != rhs.name) return false;
    RAYX_X_MACRO_RAY_ATTR
#undef X
    return true;
}

bool operator!=(const Rays& lhs, const Rays& rhs) { return !(lhs == rhs); }

}  // namespace RAYX
