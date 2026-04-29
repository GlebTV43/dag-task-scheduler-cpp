#pragma once

#include "IntegralConstant.hpp"

namespace MySTD
{
    
template<typename T>
struct IsPointer : FalseType {};

template<typename T>
struct IsPointer<T*> : TrueType {};

template<typename T>
struct IsPointer<T* const> : TrueType {};

template<typename T>
struct IsPointer<T* volatile> : TrueType {};

template<typename T>
struct IsPointer<T* const volatile> : TrueType {};

} // namespace MySTD