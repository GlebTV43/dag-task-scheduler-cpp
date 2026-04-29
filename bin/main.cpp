#include "../lib/Tuple.hpp"
#include "../lib/Apply.hpp"
#include "../lib/TTaskScheduler.hpp"
#include "../lib/TaskNode.hpp"

#include <iostream>

int main()
{
    MySTD::Tuple<int, int, int> tuple{2, 3, 4};

    int result = MySTD::Apply(
        [](int a, int b, int c)
        {
            return a * b + c;
        },
        tuple
    );

    std::cout << result << '\n';

    return 0;
}