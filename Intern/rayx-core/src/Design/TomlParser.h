#pragma once

#include <filesystem>
#include <optional>
#include "Debug/Debug.h"
#include "Data/DatFile.h"
#include "TypedTable.h"
#include "TypedVariant.h"
#include "Strings.h"

// TODO clean this up
#include "../../Extern/tomlplusplus/include/toml++/toml.hpp"

// TODO improve error messages.
// Maybe each tomlParseImpl should get an argument that expresses that path we took to this node.
// This could be dumped upon error for debugging purposes.

template <typename ...Targs>
inline void tomlParseImpl(toml::node_view<const toml::node>, std::optional<TypedTable<Targs...>>&);

template <typename ...Targs>
inline void tomlParseImpl(toml::node_view<const toml::node>, std::optional<TypedVariant<Targs...>>&);

// Every tomlParseImpl function in this file should either set the `output` to some value, or fail with RAYX_ERR.
// Keeping `output = {}` is not valid.

// TODO do we want to put this impl to DatFile.h?
inline void tomlParseImpl(toml::node_view<const toml::node> input, std::optional<DatFile>& out) {
    using Ty = TypedTable<Field<std::string, FilenameStr>>;
    std::optional<Ty> tab;
    tomlParseImpl(input, tab);
    std::string s = tab.value().field<FilenameStr>();

    DatFile d;
    DatFile::load(s, &d);
    out = d;
}

inline void tomlParseImpl(toml::node_view<const toml::node> input, std::optional<std::string>& out) {
    auto* x = input.as_string();
    if (!x) RAYX_ERR << "string parsing failed!";
    out = x->get();
}

inline void tomlParseImpl(toml::node_view<const toml::node> input, std::optional<double>& out) {
    auto* x = input.as_floating_point();
    if (x) {
        out = x->get();
        return;
    }

    auto* y = input.as_integer();
    if (y) {
        out = (double) y->get();
        return;
    }

    RAYX_ERR << "double parsing failed!";
}

inline void tomlParseImpl(toml::node_view<const toml::node> input, std::optional<int>& out) {
    auto* x = input.as_integer();
    if (!x) RAYX_ERR << "int parsing failed!";
    out = x->get();
}

template <typename T>
inline void tomlParseImpl(toml::node_view<const toml::node> input, std::optional<std::vector<T>>& out) {
    std::vector<T> ret;

    // A missing array is considered as an empty array.
    if (!input) {
        out = ret;
        return;
    }

    const toml::array* a = input.as_array();
    if (!a) RAYX_ERR << "vector parsing failed! input is no array.";
    for (auto it = a->cbegin(); it != a->cend(); it++) {
        std::optional<T> t;
        toml::node_view<const toml::node> n = (toml::node_view<const toml::node>) *it;
        tomlParseImpl(n, t);
        ret.push_back(t.value());
    }
    out = ret;
}

// This is the TypedTable implementation.
template <typename ...Targs>
inline void tomlParseImpl(toml::node_view<const toml::node> input, std::optional<TypedTable<Targs...>>& output) {
    const toml::table* tab = input.as_table();
    if (!tab) RAYX_ERR << "table parsing failed! input is no table.";

    TypedTable<Targs...> t;

    auto f = [&]<typename T>() {
        std::optional<typename T::Ty> opt_t;
        toml::node_view<const toml::node> n = (*tab)[(toml::path) T::Name::str()];
        tomlParseImpl(n, opt_t);
        t.template field<typename T::Name>() = opt_t.value();
    };
    (f.template operator()<Targs>(), ...);

    output = t;
}

// This is the TypedVariant implementation.
template <typename ...Targs>
inline void tomlParseImpl(toml::node_view<const toml::node> input, std::optional<TypedVariant<Targs...>>& out) {
    const toml::table* tab = input.as_table();
    if (!tab) RAYX_ERR << "variant parsing failed! input is no table.";
    auto* s_ = (*tab)["tag"].as_string();
    if (!s_) RAYX_ERR << "variant parsing failed! \"tag\" is no string.";
    std::string s = s_->get();

    TypedVariant<Targs...> retval;

    bool found = false;
    auto f = [&]<typename T>() {
        if (T::Tag::str() == s) {
            if (found) {
                RAYX_ERR << "variant parsing failed: multiple matches for type \"" << s << "\"";
            }
            std::optional<typename T::Ty> c;
            tomlParseImpl(input, c);
            retval.template set<typename T::Tag>(c.value());
            out = retval;

            found = true;
        }
    };
    (f.template operator()<Targs>(), ...);

    if (!found) {
        RAYX_ERR << "variant parsing failed: no ident matches string \"" << s << "\"";
    }
}

// The tomlParseImpl base implementation.
// This should only work, if T is a subclass of either TypedTable<...> or TypedVariant<...>.
template <typename T, typename = std::enable_if_t<!std::is_same_v<T, typename T::TypedBase>>>
inline void tomlParseImpl(toml::node_view<const toml::node> input, std::optional<T>& output) {
    std::optional<typename T::TypedBase> opt;
    tomlParseImpl(input, opt);
    T t;
    *static_cast<typename T::TypedBase*>(&t) = opt.value();
    output = t;
}

// The central function that should be called from the outside of this module.
template <typename T>
inline T tomlParse(const std::filesystem::path& p) {
    auto tomldata = toml::parse_file(std::string(p));
    toml::node_view<const toml::node> view = (toml::node_view<const toml::node>) tomldata;
    std::optional<T> opt_t;
    tomlParseImpl(view, opt_t);
    return opt_t.value();
}
