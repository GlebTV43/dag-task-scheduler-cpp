#pragma once

#include "IntegralConstant.hpp"

namespace MySTD
{

template<typename T>
struct IsConst : FalseType {};

template<typename T>
struct IsConst<const T> : TrueType {};

} // namespace MySTD