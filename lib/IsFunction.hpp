#pragma once

#include "IntegralConstant.hpp"
#include "RemoveReference.hpp"
#include "IsConst.hpp"
#include "IsReference.hpp"

namespace MySTD
{

template<typename T>
struct IsFunction : BoolConstant<!IsConst<const T>::value && !IsReference<T>::value> {};

template<typename T>
struct IsMemberFunctionPointerHelp : FalseType {};

template<typename T, typename U>
struct IsMemberFunctionPointerHelp<T U::*> : IsFunction<T> {};

template<typename T>
struct IsMemberFunctionPointer : IsMemberFunctionPointerHelp<RemoveCVRefType<T>> {};

template<typename T>
inline constexpr bool IsMemberFunctionPointerV = IsMemberFunctionPointer<T>::value;

template<typename T>
struct IsMemberPointerHelp : FalseType {};

template<typename T, typename U>
struct IsMemberPointerHelp<T U::*> : TrueType {};

template<typename T, typename U>
struct IsMemberPointerHelp<T U::* const> : TrueType {};

template<typename T, typename U>
struct IsMemberPointerHelp<T U::* volatile> : TrueType {};

template<typename T, typename U>
struct IsMemberPointerHelp<T U::* const volatile> : TrueType {};

template<typename T>
struct IsMemberPointer : IsMemberPointerHelp<RemoveCVRefType<T>> {};

template<typename T>
inline constexpr bool IsMemberPointerV = IsMemberPointer<T>::value;

template<typename T>
struct IsMemberObjectPointer : BoolConstant<IsMemberPointerV<T> && !IsMemberFunctionPointer<T>::value> {};

template<typename T>
inline constexpr bool IsMemberObjectPointerV = IsMemberObjectPointer<T>::value;

} // namespace MySTD