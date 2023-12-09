#pragma once

#include <variant>
#include <memory>
#include <Debug/Debug.h>

template <typename TTy, typename TTag>
struct Case {
    using Ty = TTy;
    using Tag = TTag;
};

template <typename Tag, typename Head, typename ...Targs>
constexpr size_t findVariantIndexOfTag()
{
    if constexpr (std::is_same_v<Tag, typename Head::Tag>) {
        return 0;
    } else {
        return 1 + findVariantIndexOfTag<Tag, Targs...>();
    }
}

template <typename ...Targs>
struct TypedVariant {
    // unique_ptr might make more sense, but then we don't have a copy-constructor for it.
    using TypedBase = TypedVariant<Targs...>;

    template <typename Tag>
    inline void set(auto v) {
        std::shared_ptr<decltype(v)> ptr = std::make_shared<decltype(v)>(v);
        decltype(m_variant) tmp_variant(std::in_place_index<findVariantIndexOfTag<Tag, Targs...>()>, ptr);
        m_variant = tmp_variant;
    }

    // checks whether 
    template <typename Tag>
    inline bool is() const {
        return std::get_if<findVariantIndexOfTag<Tag, Targs...>()>(&m_variant) != nullptr;
    }

    template <typename Tag>
    inline auto& as() {
        if (!is<Tag>()) {
            RAYX_ERR << "trying to access inactive case!";
        }
        return *std::get<findVariantIndexOfTag<Tag, Targs...>()>(m_variant);
    }

    template <typename Tag>
    inline const auto& as() const {
        if (!is<Tag>()) {
            RAYX_ERR << "trying to access inactive case!";
        }
        return *std::get<findVariantIndexOfTag<Tag, Targs...>()>(m_variant);
    }

    private:
    std::variant<std::shared_ptr<typename Targs::Ty> ...> m_variant;
};
