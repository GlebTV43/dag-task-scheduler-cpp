#pragma once

#include <utility>
#include "SizeT.hpp"
#include "Tuple.hpp"

namespace MySTD 
{

template<typename T, T... Int>
struct IntegerSequence 
{
    using value_type = T;

    static constexpr SizeT Size() noexcept
    {
        return sizeof...(Int);
    }
};

template<SizeT... Indexes>
using IndexSequence = IntegerSequence<SizeT, Indexes...>;

/*template<typename T, T I>
using MakeIntergerSequence = IntergerSequence<T, I>; */

namespace helper
{
    template<typename T, T Index, T N, T... Ints>
    struct MakeIntegerSequence
    {
        using type = typename MakeIntegerSequence<T, Index + 1, N, Ints..., Index>::type;
    };

    template<typename T, T N, T... Ints>
    struct MakeIntegerSequence<T, N, N, Ints...>
    {
        using type = IntegerSequence<T, Ints...>;
    };
} // namespace helper

template<typename T, T N>
using MakeIntegerSequence = typename helper::MakeIntegerSequence<T, 0, N>::type;

template<SizeT Index>
using MakeIndexSequence = MakeIntegerSequence<SizeT, Index>;

template<typename... N>
using IndexSequenceFor = MakeIndexSequence<sizeof...(N)>;

template<typename Func, typename Tuple, SizeT... I>
constexpr decltype(auto) ApplyImpl(Func&& func, Tuple&& tuple, IndexSequence<I...>)
{
    return std::forward<Func>(func)(Get<I>(std::forward<Tuple>(tuple))...);
}

template<typename Func, typename Tuple>
constexpr decltype(auto) Apply(Func&& func, Tuple&& tuple)
{
    using CleanTupleType = RemoveCVType<RemoveReferenceType<Tuple>>;

    return ApplyImpl(std::forward<Func>(func), std::forward<Tuple>(tuple), MakeIndexSequence<TupleSizeV<CleanTupleType>>{});
}   


} // namespace MySTD