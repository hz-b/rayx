#include "ObjectMask.h"

#include <algorithm>
#include <format>
#include <memory>
#include <stdexcept>
#include <vector>

namespace rayx::detail {

std::unique_ptr<bool[]> toRawObjectMask(const ObjectMaskNoneType, const int numObjects) {
    auto result = std::make_unique<bool[]>(numObjects);
    std::fill_n(result.get(), numObjects, false);
    return result;
}

std::unique_ptr<bool[]> toRawObjectMask(const ObjectMaskAllType, const int numObjects) {
    auto result = std::make_unique<bool[]>(numObjects);
    std::fill_n(result.get(), numObjects, true);
    return result;
}

std::unique_ptr<bool[]> toRawObjectMask(const ObjectMaskByObjectIds& byObjectIds, const int numObjects) {
    auto result = std::make_unique<bool[]>(numObjects);
    for (int index : byObjectIds.objectIds) {
        if (index < 0 || index >= numObjects)
            throw std::out_of_range(std::format("object id `{}` is out of range [0, numObjects), where numObjects is `{}`", index, numObjects));
        result[index] = true;
    }
    return result;
}

std::unique_ptr<bool[]> toRawObjectMask(const ObjectMask& mask, const int numObjects) {
    return std::visit([=](const auto& arg) { return toRawObjectMask(arg, numObjects); }, mask);
}

}  // namespace rayx::detail
