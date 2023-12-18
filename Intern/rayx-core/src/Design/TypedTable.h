#pragma once

#include <tuple>

template <typename TTy, typename TName>
struct Field {
    using Ty = TTy;
    using Name = TName;
};

template <typename Name, typename Head, typename ...Targs>
constexpr size_t findTupleIndexOfName()
{
    if constexpr (std::is_same_v<Name, typename Head::Name>) {
        return 0;
    } else {
        return 1 + findTupleIndexOfName<Name, Targs...>();
    }
}


template <typename ...Targs>
struct TypedTable {
    using TypedBase = TypedTable<Targs...>;

    template <typename Name>
    inline auto& field() {
        return std::get<findTupleIndexOfName<Name, Targs...>()>(m_tuple);
    }

    template <typename Name>
    inline const auto& field() const {
        return std::get<findTupleIndexOfName<Name, Targs...>()>(m_tuple);
    }

    private:
    std::tuple<typename Targs::Ty ...> m_tuple;
};
