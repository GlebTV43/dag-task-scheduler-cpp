#pragma once

#include <utility>

#include "RemoveReference.hpp"
#include "SizeT.hpp"
#include "IntegralConstant.hpp"

namespace MySTD
{

template<SizeT Index, typename T>
class TupleImpl
{  
    T value_;
public: 
    TupleImpl() = default; // добавлю во все классы дефолтный конструктор, чтобы можно было создавать пустой Tuple,
                           // но это будет работать только для типов, которые умеют конструироваться по умолчанию
    TupleImpl(T const &value) : value_(value) {}

    TupleImpl(T&& value) : value_(std::move(value)) {}

    T& Get()
    {
        return value_;
    }
};

template<SizeT Index, typename... Types>
class TupleRecursiveBase 
{
public:
    TupleRecursiveBase() = default;
};

template<SizeT Index, typename L, typename... Types>
class TupleRecursiveBase<Index, L, Types...> : public TupleImpl<Index, typename MySTD::RemoveReferenceType<L>>,
                                                public TupleRecursiveBase<Index + 1, Types...> 
{
public:
    TupleRecursiveBase() = default;

    template<typename T, typename... Args>
    TupleRecursiveBase(T&& arg, Args&&... args) : TupleImpl<Index, typename MySTD::RemoveReferenceType<T>>(std::forward<T>(arg)),
                                                    TupleRecursiveBase<Index + 1,  Types...>(std::forward<Args>(args)...) {}
};

template<typename... Types>
class Tuple;

template<>
class Tuple<>
{
public:
    Tuple() {}
};

template<typename T, typename... Types>
class Tuple<T, Types...> : public TupleRecursiveBase<0, T, Types...>
{
public: 
    Tuple() = default;

    template<typename... Args>
    Tuple(Args&&... args) : TupleRecursiveBase<0, T, Types...>(std::forward<Args>(args)...) {}
    
    template<typename... Args>
    friend bool operator==(Tuple<Args...>& tuple1, Tuple<Args...>& tuple2);
};

template<SizeT Index, typename T, typename... Types>
struct ExtractTypeAt
{
    using type = typename ExtractTypeAt<Index - 1, Types...>::type;
};

template<typename T, typename... Types>
struct ExtractTypeAt<0, T, Types...>
{
    using type = T;
};

template<SizeT Index, typename... Types>
using ExtractTypeAtType = typename ExtractTypeAt<Index, Types...>::type;

template<SizeT Index, typename... Args>
auto& Get(Tuple<Args...>& tuple) 
{
    using CurType = ExtractTypeAtType<Index, Args...>;

    return static_cast<TupleImpl<Index, CurType>&>(tuple).Get();
};

template<SizeT Index, typename... Args>
bool CompareTuples(const Tuple<Args...>& tuple1, const Tuple<Args...>& tuple2)
{
    if constexpr (Index == 0)
    {
        return Get<0>(tuple1) == Get<0>(tuple2);
    } 
    else 
    {
        return Get<Index>(tuple1) == Get<Index>(tuple2) && CompareTuples<Index - 1, Args...>(tuple1, tuple2);
    }
}

template<typename... Args>
bool operator==(const Tuple<Args...>& tuple1, const Tuple<Args...>& tuple2)
{
    return CompareTuples<sizeof...(Args) - 1, Args...>(tuple1, tuple2);
}

template<typename T>
struct TupleSize;

template<typename... Types>
struct TupleSize<Tuple<Types...>> : 
    IntegralConstant<SizeT, sizeof...(Types)> {};

template<typename T>
constexpr SizeT TupleSizeV = TupleSize<T>::value;

} // namespace MySTD