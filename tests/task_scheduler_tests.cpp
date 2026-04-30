#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "../lib/TTaskScheduler.hpp"
#include "../lib/Exception.hpp"

TEST(TaskSchedulerExtra, LazyGetResultSyncDoesNotExecuteUnrelatedTasks)
{
    TTaskScheduler scheduler;

    int unrelatedRuns = 0;

    auto needed = scheduler.Add([] {
        return 10;
    });

    auto unused = scheduler.Add([&unrelatedRuns] {
        ++unrelatedRuns;
        return 100;
    });

    auto resultTask = scheduler.Add([](const int& x) {
        return x + 5;
    }, needed.GetFutureResult<const int&>());

    ASSERT_EQ(resultTask.GetResultSync<int>(), 15);
    ASSERT_EQ(unrelatedRuns, 0);

    (void)unused;
}

TEST(TaskSchedulerExtra, ExecutesEachTaskOnlyOnceEvenIfFutureUsedTwice)
{
    TTaskScheduler scheduler;

    int runs = 0;

    auto source = scheduler.Add([&runs] {
        ++runs;
        return 21;
    });

    auto firstFuture = source.GetFutureResult<const int&>();
    auto secondFuture = source.GetFutureResult<const int&>();

    auto combined = scheduler.Add([](const int& a, const int& b) {
        return a + b;
    }, firstFuture, secondFuture);

    ASSERT_EQ(combined.GetResultSync<int>(), 42);
    ASSERT_EQ(runs, 1);

    ASSERT_EQ(combined.GetResultSync<const int&>(), 42);
    ASSERT_EQ(runs, 1);
}

TEST(TaskSchedulerExtra, ReferenceFutureCanBeReadMultipleTimes)
{
    TTaskScheduler scheduler;

    auto task = scheduler.Add([] {
        return std::string("stable");
    });

    auto future = task.GetFutureResult<const std::string&>();

    const std::string& first = future.Get();
    const std::string& second = future.Get();

    ASSERT_EQ(first, "stable");
    ASSERT_EQ(second, "stable");
    ASSERT_EQ(&first, &second);
}

TEST(TaskSchedulerExtra, MoveFutureCanBeReadOnlyOnce)
{
    TTaskScheduler scheduler;

    auto task = scheduler.Add([] {
        return std::string("move-only-result");
    });

    auto future = task.GetFutureResult<std::string>();

    std::string value = future.Get();

    ASSERT_EQ(value, "move-only-result");
    ASSERT_THROW(future.Get(), MySTD::ResultAlreadyMovedException);
}

TEST(TaskSchedulerExtra, SupportsMoveOnlyResultThroughFuture)
{
    TTaskScheduler scheduler;

    auto source = scheduler.Add([] {
        return std::make_unique<int>(55);
    });

    auto result = scheduler.Add([](std::unique_ptr<int> value) {
        return *value + 1;
    }, source.GetFutureResult<std::unique_ptr<int>>());

    ASSERT_EQ(result.GetResultSync<int>(), 56);
}

TEST(TaskSchedulerExtra, ExecuteAllRunsVoidTasks)
{
    TTaskScheduler scheduler;

    int value = 0;

    scheduler.Add([&value] {
        value = 42;
    });

    scheduler.ExecuteAll();

    ASSERT_EQ(value, 42);
}

TEST(TaskSchedulerExtra, ExecuteAllRespectsDependencies)
{
    TTaskScheduler scheduler;

    std::vector<int> order;

    auto first = scheduler.Add([&order] {
        order.push_back(1);
        return 10;
    });

    auto second = scheduler.Add([&order](const int& x) {
        order.push_back(2);
        return x + 5;
    }, first.GetFutureResult<const int&>());

    auto third = scheduler.Add([&order](const int& x) {
        order.push_back(3);
        return x * 2;
    }, second.GetFutureResult<const int&>());

    scheduler.ExecuteAll();

    ASSERT_THAT(order, ::testing::ElementsAre(1, 2, 3));
    ASSERT_EQ(third.GetResultSync<const int&>(), 30);
}

TEST(TaskSchedulerExtra, SupportsMemberFunctionTaskOnObject)
{
    struct Accumulator
    {
        int base = 0;

        int Add(int x) const
        {
            return base + x;
        }
    };

    TTaskScheduler scheduler;

    Accumulator accumulator{30};

    auto task = scheduler.Add(&Accumulator::Add, accumulator, 12);

    ASSERT_EQ(task.GetResultSync<int>(), 42);
}

TEST(TaskSchedulerExtra, SupportsMemberFunctionTaskOnPointer)
{
    struct Accumulator
    {
        int base = 0;

        int Add(int x) const
        {
            return base + x;
        }
    };

    TTaskScheduler scheduler;

    Accumulator accumulator{35};

    auto task = scheduler.Add(&Accumulator::Add, &accumulator, 7);

    ASSERT_EQ(task.GetResultSync<int>(), 42);
}

TEST(TaskSchedulerExtra, ApplyPassesResultToNextTaskByValueOrRvalue)
{
    TTaskScheduler scheduler;

    auto first = scheduler.Add([] {
        return std::string("hello");
    });

    auto second = first.Apply([](std::string value) {
        value += " world";
        return value.size();
    });

    ASSERT_EQ(second.GetResultSync<std::size_t>(), 11u);
}

TEST(TaskSchedulerExtra, ApplyPassesResultToNextTaskByLvalueReference)
{
    TTaskScheduler scheduler;

    auto first = scheduler.Add([] {
        return std::string("hello");
    });

    auto second = first.Apply([](std::string& value) {
        value += "!";
        return value;
    });

    ASSERT_EQ(second.GetResultSync<std::string>(), "hello!");
}

TEST(TaskSchedulerExtra, ApplyCanReturnVoid)
{
    TTaskScheduler scheduler;

    int observed = 0;

    auto first = scheduler.Add([] {
        return 42;
    });

    auto second = first.Apply([&observed](int value) {
        observed = value;
    });

    scheduler.ExecuteAll();

    ASSERT_EQ(observed, 42);

    (void)second;
}
