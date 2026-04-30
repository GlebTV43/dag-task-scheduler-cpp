#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <utility>

#include "../lib/Tuple.hpp"
#include "../lib/Apply.hpp"
#include "../lib/Invoke.hpp"

TEST(MySTD_Tuple, StoresAndGetsDifferentTypes)
{
    MySTD::Tuple<int, double, std::string> tuple(10, 2.5, std::string("hello"));

    ASSERT_EQ(MySTD::Get<0>(tuple), 10);
    ASSERT_DOUBLE_EQ(MySTD::Get<1>(tuple), 2.5);
    ASSERT_EQ(MySTD::Get<2>(tuple), "hello");

    MySTD::Get<0>(tuple) = 42;
    MySTD::Get<2>(tuple) += " world";

    ASSERT_EQ(MySTD::Get<0>(tuple), 42);
    ASSERT_EQ(MySTD::Get<2>(tuple), "hello world");
}

TEST(MySTD_Tuple, SupportsEmptyTupleInApply)
{
    MySTD::Tuple<> tuple;

    auto result = MySTD::Apply([] {
        return 123;
    }, tuple);

    ASSERT_EQ(result, 123);
}

TEST(MySTD_Apply, ExpandsTupleIntoCallable)
{
    MySTD::Tuple<int, int, int> tuple(2, 3, 4);

    auto result = MySTD::Apply([](int a, int b, int c) {
        return a * 100 + b * 10 + c;
    }, tuple);

    ASSERT_EQ(result, 234);
}

TEST(MySTD_Apply, PassesTupleElementsByReference)
{
    int value = 10;
    MySTD::Tuple<int> tuple(value);

    MySTD::Apply([](int& x) {
        x += 5;
    }, tuple);

    ASSERT_EQ(MySTD::Get<0>(tuple), 15);
}

TEST(MySTD_Invoke, InvokesFreeFunctionLikeObject)
{
    struct Functor
    {
        int operator()(int a, int b) const
        {
            return a - b;
        }
    };

    ASSERT_EQ(MySTD::Invoke(Functor{}, 10, 3), 7);
}

TEST(MySTD_Invoke, InvokesMemberFunctionOnObject)
{
    struct Counter
    {
        int value = 0;

        int Add(int x) const
        {
            return value + x;
        }
    };

    Counter counter{10};

    ASSERT_EQ(MySTD::Invoke(&Counter::Add, counter, 5), 15);
}

TEST(MySTD_Invoke, InvokesMemberFunctionOnPointer)
{
    struct Counter
    {
        int value = 0;

        int Add(int x) const
        {
            return value + x;
        }
    };

    Counter counter{20};

    ASSERT_EQ(MySTD::Invoke(&Counter::Add, &counter, 7), 27);
}

TEST(MySTD_Invoke, InvokesMemberDataOnObject)
{
    struct Point
    {
        int x = 0;
        int y = 0;
    };

    Point point{3, 4};

    ASSERT_EQ(MySTD::Invoke(&Point::x, point), 3);

    MySTD::Invoke(&Point::y, point) = 10;
    ASSERT_EQ(point.y, 10);
}

TEST(MySTD_Invoke, InvokesMemberDataOnPointer)
{
    struct Point
    {
        int x = 0;
    };

    Point point{42};

    ASSERT_EQ(MySTD::Invoke(&Point::x, &point), 42);

    MySTD::Invoke(&Point::x, &point) = 100;
    ASSERT_EQ(point.x, 100);
}
