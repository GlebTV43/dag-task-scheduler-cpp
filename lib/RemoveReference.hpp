#pragma once

namespace MySTD
{

template<typename T>
struct RemoveReference
{
    using type = T;
};

template<typename T>
struct RemoveReference<T&>
{
    using type = T;
};

template<typename T>
struct RemoveReference<T&&>
{
    using type = T;
};

template<typename T>
using RemoveReferenceType = typename RemoveReference<T>::type;


template<typename T>
struct RemoveCV
{
    using type = T;
};

template<typename T>
struct RemoveCV<const T>
{
    using type = T;
};

template<typename T>
struct RemoveCV<volatile T>
{
    using type = T;
};

template<typename T>
struct RemoveCV<const volatile T>
{
    using type = T;
};

template<typename T>
using RemoveCVType = typename RemoveCV<T>::type;

} // namespace MySTD