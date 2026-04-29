#pragma once

#include <utility>
#include "SizeT.hpp"
#include "Tuple.hpp"
#include "IntergerSequence.hpp"
#include "Invoke.hpp"

namespace MySTD 
{

template<typename Func, typename Tuple, SizeT... I>
constexpr decltype(auto) ApplyImpl(Func&& func, Tuple&& tuple, IndexSequence<I...>)
{
    return Invoke(std::forward<Func>(func), Get<I>(std::forward<Tuple>(tuple))...);
}

template<typename Func, typename Tuple>
constexpr decltype(auto) Apply(Func&& func, Tuple&& tuple)
{
    using CleanTupleType = RemoveCVType<RemoveReferenceType<Tuple>>;

    return ApplyImpl(std::forward<Func>(func), std::forward<Tuple>(tuple), MakeIndexSequence<TupleSizeV<CleanTupleType>>{});
}   


} // namespace MySTD