#pragma once

#include "IntegralConstant.hpp"

namespace MySTD
{

template<typename T, typename U>
struct IsSame : FalseType {};

template<typename T>
struct IsSame<T, T> : TrueType {};

template<typename T, typename U>
inline constexpr bool IsSameV = IsSame<T, U>::value;

} // namespace MySTD