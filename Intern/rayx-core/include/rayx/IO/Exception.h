#pragma once

#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include "rayx/Core.h"

namespace rayx::detail {

// TODO: create exception builder that includes git revision, file and line number, and an optional note.

constexpr inline std::string formatNote(const std::optional<std::string_view> note) {
    if (note) return std::format(". note: {}", *note);
    return "";
}

constexpr std::exception unimplementedCodePathException(const std::optional<std::string_view> note = std::nullopt) {
    return std::runtime_error(
        std::format("unimplemented code path. this should never happen, please report to the developers. the error occoured in gitrev: {}{}",
                    RAYX_GIT_REVISION, formatNote(note)));
}

}  // namespace rayx::detail
