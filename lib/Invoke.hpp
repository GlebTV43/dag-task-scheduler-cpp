#pragma once 

#include <utility>

#include "RemoveReference.hpp"
#include "IntegralConstant.hpp"
#include "IsPointer.hpp"
#include "IsFunction.hpp"

namespace MySTD 
{

template<typename Func, typename... Args>
concept CallableObj = requires(Func&& func, Args&&... args) 
{
    std::forward<Func>(func)(std::forward<Args>(args)...);
} && !IsPointer<RemoveCVRefType<Func>>::value;

template<typename Func, typename... Args>
concept CallableObjAndPointer = requires(Func&& func, Args&&... args) 
{
    std::forward<Func>(func)(std::forward<Args>(args)...);
} && IsPointer<RemoveCVRefType<Func>>::value;

template<typename T>
concept IsPointerObject = IsPointer<RemoveCVRefType<T>>::value;

template<typename T>
concept NotPointerObject = !IsPointer<RemoveCVRefType<T>>::value;

template<typename T>
concept MemberFunctionPointer = IsMemberFunctionPointerV<RemoveCVRefType<T>>;

template<typename T>
concept MemberObjectPointer = IsMemberObjectPointerV<RemoveCVRefType<T>>;

template<typename Func, typename... Args>
requires CallableObj<Func, Args...>
decltype(auto) Invoke(Func&& func, Args&&... args)
{
    return (std::forward<Func>(func))(std::forward<Args>(args)...);
}

template<typename Func, typename... Args>
requires CallableObjAndPointer<Func, Args...>
decltype(auto) Invoke(Func&& func, Args&&... args)
{
    return (*std::forward<Func>(func))(std::forward<Args>(args)...);
}

template<MemberFunctionPointer Func, NotPointerObject Object, typename... Args>
decltype(auto) Invoke(Func func, Object&& obj, Args&&... args)
{
    return (std::forward<Object>(obj).*func)(std::forward<Args>(args)...);
}

template<MemberFunctionPointer Func, IsPointerObject Object, typename... Args>
decltype(auto) Invoke(Func func, Object&& obj, Args&&... args)
{
    return ((*std::forward<Object>(obj)).*func)(std::forward<Args>(args)...);
}

template<MemberObjectPointer Func, NotPointerObject Object>
decltype(auto) Invoke(Func func, Object&& obj)
{
    return std::forward<Object>(obj).*func;
}

template<MemberObjectPointer Func, IsPointerObject Object>
decltype(auto) Invoke(Func func, Object&& obj)
{
    return (*std::forward<Object>(obj)).*func;

}

} // namespace MySTD