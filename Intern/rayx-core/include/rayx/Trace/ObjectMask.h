#pragma once

#include <memory>
#include <variant>
#include <vector>

namespace rayx {

/// record none of the sources and elements
struct ObjectMaskNoneType {};
inline constexpr ObjectMaskNoneType ObjectMaskNone;

/// record all the sources and elements
struct ObjectMaskAllType {};
inline constexpr ObjectMaskAllType ObjectMaskAll;

/// record the sources and elements with the specified object indices
/// sources and elements are indexed together
/// the object ids can be retreived in `Beamline` class
struct ObjectMaskByObjectIds {
    std::vector<int> objectIds;
};

using ObjectMask = std::variant<ObjectMaskNoneType, ObjectMaskAllType, ObjectMaskByObjectIds>;

}  // namespace rayx

namespace rayx::detail {

std::unique_ptr<bool[]> toRawObjectMask(const ObjectMaskNoneType, const int numObjects);
std::unique_ptr<bool[]> toRawObjectMask(const ObjectMaskAllType, const int numObjects);
std::unique_ptr<bool[]> toRawObjectMask(const ObjectMaskByObjectIds& byObjectIds, const int numObjects);
std::unique_ptr<bool[]> toRawObjectMask(const ObjectMask& mask, const int numObjects);

}  // namespace rayx::detail
