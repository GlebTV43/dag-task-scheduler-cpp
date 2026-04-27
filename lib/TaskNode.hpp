#pragma once

#include <memory>

#include "Exception.hpp"
#include "Tuple.hpp"

enum class Status 
{
    kNotStarted,
    kExecuting,
    kFinished
};

class TaskNodeBase 
{
protected:
    Status state_ = Status::kNotStarted;
public:

    virtual void Execute() = 0;
    virtual ~TaskNodeBase() {};
};

template<typename ResultType, typename Func, typename... Args>
class TaskNode : public TaskNodeBase
{
    Func func_;
    MySTD::Tuple<Args...> args_;
    std::unique_ptr<ResultType> result_;
    bool result_is_moved_ = false;

public:
    template<typename AnotherFunc, typename... AArgs> 
    TaskNode(AnotherFunc func, AArgs... args) : func_(std::move(func)), args_(std::forward<AArgs>(args)...) {}

    void Execute() override
    {
        if (state_ == Status::kFinished)
        {
            return;
        }
        else if (state_ == Status::kExecuting)
        {
            throw DependencyCycleException();
        }

        state_ = Status::kExecuting;

        template<SizeT... Indexes>
        result_ = func_(Get<Indexes>(args_)...);
        
    }
};