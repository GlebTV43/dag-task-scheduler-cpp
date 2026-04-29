#pragma once

#include "IntegralConstant.hpp"

namespace MySTD
{

template<typename T>
struct IsReference : FalseType {};

template<typename T>
struct IsReference<T&> : TrueType {};

template<typename T>
struct IsReference<T&&> : TrueType {};

} // namespace MySTD