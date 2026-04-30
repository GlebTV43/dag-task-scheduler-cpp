#pragma once

#include <vector>
#include <memory>
#include <utility>

#include "TaskNode.hpp"
#include "TTask.hpp"
#include "Invoke.hpp"
#include "RemoveReference.hpp"

class TTaskScheduler 
{
    std::vector<std::unique_ptr<TaskNodeBase>> tasks_;
public:

    template<typename Func, typename... Args>
    auto Add(Func&& func, Args&&... args)
    {
        using ResultType = decltype(MySTD::Invoke(func, Resolve(args)...));

        using CleanFunc = MySTD::RemoveReferenceType<Func>;

        using Node = TaskNode<ResultType, 
                              CleanFunc,
                              MySTD::RemoveReferenceType<Args>...>;
        
        std::unique_ptr<Node> node(new Node(std::forward<Func>(func), std::forward<Args>(args)...));
            // таким образом решил проблему утечки, меняется лишь владение node, node_result - валиден
        TaskNodeResult<ResultType>* node_result = node.get();

        tasks_.push_back(std::move(node));

        return TTask<ResultType>(node_result, this);
    }

    void ExecuteAll()
    {
        for (auto& task : tasks_)
        {
            task->Execute();
        }
    }
};

template<typename ResultType>
    template<typename Func>
        requires ApplyNeedLValue<Func, ResultType>
auto TTask<ResultType>::Apply(Func&& func)
{
    auto future = GetFutureResult<ResultType&>();
    return scheduler_->Add(func, future);
}

template<typename ResultType>
    template<typename Func>
        requires (!ApplyNeedLValue<Func, ResultType> &&
                    CanInvoke<MySTD::RemoveReferenceType<Func>&, ResultType&&>)
auto TTask<ResultType>::Apply(Func&& func)
{
    auto future = GetFutureResult<ResultType>();
    return scheduler_->Add(func, future);
}