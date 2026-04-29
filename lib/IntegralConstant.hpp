#pragma once

namespace MySTD
{

template<typename T, T V>
struct IntegralConstant
{
    static constexpr T value = V;
    using value_type = T;
    using type = IntegralConstant;
    constexpr operator value_type() const noexcept 
    {
        return value;
    }
    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};

template<bool B>
using BoolConstant = IntegralConstant<bool, B>;

using TrueType = BoolConstant<true>;

using FalseType = BoolConstant<false>;

} // namespace MySTD