#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../lib/TTaskScheduler.hpp"

TEST(BaseThings, WithoutArguments)
{
    TTaskScheduler scheduler;

    auto task = scheduler.Add([] {
        return 5;
    });

    int result = task.GetResultSync<int>();
    ASSERT_EQ(result, 5);
}

TEST(BaseThings, WithArguments)
{
    TTaskScheduler scheduler;

    auto task = scheduler.Add([](int a, int b) {
        return a + b;
    }, 2, 3);

    int result = task.GetResultSync<int>();
    ASSERT_EQ(result, 5);
}

TEST(BaseThings, ExecuteAll)
{
    TTaskScheduler scheduler;

    auto task = scheduler.Add([] {
        return 5;
    });

    scheduler.ExecuteAll();

    int result = task.GetResultSync<int>();
    ASSERT_EQ(result, 5);
}

TEST(BaseThings, FutureDependency)
{
    TTaskScheduler scheduler;

    auto first = scheduler.Add([] {
        return 10;
    });

    auto future = first.GetFutureResult<const int&>();

    auto second = scheduler.Add([](const int& value) {
        return value + 5;
    }, future);

    int result = second.GetResultSync<int>();
    ASSERT_EQ(result, 15);
}

#include <fstream>

TEST(BaseThings, ReadMeTest)
{
    {
        std::ofstream students("students_test.csv");
        students << "isu,gpa\n";
        students << "501601,0\n";
        students << "501602,80\n";
        students << "501603,100\n";
        students << "501604,100\n";
    }

    {
        std::ofstream groups("groups_test.csv");
        groups << "group,isu\n";
        groups << "M3102,501601\n";
        groups << "M3102,501602\n";
        groups << "M3100,501603\n";
        groups << "M3100,501604\n";
    }

    TTaskScheduler scheduler;

    auto studentsTask = scheduler.Add(
        [](const std::string& filePath) {
            std::ifstream file(filePath);
            std::string line;
            std::getline(file, line); // skip header

            std::unordered_map<int, int> gpaByIsu;

            while (std::getline(file, line))
            {
                std::stringstream ss(line);

                std::string isuStr;
                std::string gpaStr;

                std::getline(ss, isuStr, ',');
                std::getline(ss, gpaStr, ',');

                int isu = std::stoi(isuStr);
                int gpa = std::stoi(gpaStr);
                gpaByIsu[isu] = gpa;
            }

            return gpaByIsu;
        },
        std::string("students_test.csv")
    );

    auto groupsTask = scheduler.Add(
        [](const std::string& filePath) {
            std::ifstream file(filePath);
            std::string line;
            std::getline(file, line); // skip header

            std::unordered_map<std::string, std::vector<int>> groupToIsus;

            while (std::getline(file, line))
            {
                std::stringstream ss(line);

                std::string group;
                int isu = 0;

                std::getline(ss, group, ',');
                ss >> isu;

                groupToIsus[group].push_back(isu);
            }

            return groupToIsus;
        },
        std::string("groups_test.csv")
    );

    auto studentsFuture =
        studentsTask.GetFutureResult<const std::unordered_map<int, int>&>();

    auto groupsFuture =
        groupsTask.GetFutureResult<std::unordered_map<std::string, std::vector<int>>>();

    auto resultTask = scheduler.Add(
        [](
            const std::unordered_map<int, int>& gpaByIsu,
            std::unordered_map<std::string, std::vector<int>> groupToIsus
        ) {
            std::unordered_map<std::string, int> gpaByGroup;

            for (const auto& [group, isus] : groupToIsus)
            {
                int sum = 0;
                int count = 0;

                for (int isu : isus)
                {
                    auto it = gpaByIsu.find(isu);
                    if (it != gpaByIsu.end())
                    {
                        sum += it->second;
                        ++count;
                    }
                }

                if (count != 0)
                {
                    gpaByGroup[group] = sum / count;
                }
            }

            return gpaByGroup;
        },
        studentsFuture,
        groupsFuture
    );

    const auto& result =
        resultTask.GetResultSync<const std::unordered_map<std::string, int>&>();

    ASSERT_EQ(result.at("M3102"), 40);
    ASSERT_EQ(result.at("M3100"), 100);
    ASSERT_EQ(result.size(), 2);
}
