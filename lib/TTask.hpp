#pragma once

#include "TaskNode.hpp"

class TTaskScheduler;


template<typename T>
class TFuture // это считай обертка над TaskNodeResult 
{

    TaskNodeResult<T>* node_;

public:

    explicit TFuture(TaskNodeResult<T>* node) : node_(node) {}
    
    T Get()
    {
        return node_->MoveResult(); 
    }
};

template<typename T>
class TFuture<T&>
{

    TaskNodeResult<T>* node_;

public:

    explicit TFuture(TaskNodeResult<T>* node) : node_(node) {}
    
    T& Get()
    {
        return node_->GetResultRef();
    }
};

template<typename T>
class TFuture<const T&>
{

    TaskNodeResult<T>* node_;

public:

    explicit TFuture(TaskNodeResult<T>* node) : node_(node) {}
    
    const T& Get()
    {
        return node_->GetConstResultRef();
    }
};

template<typename T>
decltype(auto) Resolve(T&& value)
{
    return value;
}

template<typename T>
decltype(auto) Resolve(TFuture<T>& value)
{
    return value.Get();
}

template<typename ResultType, typename Func, typename... Args>
class TaskNode : public TaskNodeResult<ResultType>
{
    Func func_;
    MySTD::Tuple<Args...> args_;

    template<MySTD::SizeT... I>
    void ExecuteImpl(MySTD::IndexSequence<I...>)
    {
        if constexpr (!MySTD::IsSameV<ResultType, void>) 
        {
            this->SetResult(MySTD::Invoke(func_, Resolve(Get<I>(args_))...));
        }
        else 
        {
            MySTD::Invoke(func_, Resolve(Get<I>(args_))...);
            this->SetResult();
        }
    }

public:
    template<typename AnotherFunc, typename... AArgs> 
    TaskNode(AnotherFunc&& func, AArgs&&... args) : func_(std::forward<Func>(func)), args_(std::forward<AArgs>(args)...) {}

    void Execute() override
    {
        if (this->state_ == Status::kFinished)
        {
            return;
        }
        else if (this->state_ == Status::kExecuting)
        {
            throw MySTD::DependencyCycleException();
        }

        this->state_ = Status::kExecuting;

        ExecuteImpl(MySTD::MakeIndexSequence<sizeof...(Args)>{});

        this->state_ = Status::kFinished;
    }
};

template<typename Func, typename Arg>
concept CanInvoke = requires(Func func, Arg arg) {
    MySTD::Invoke(std::forward<Func>(func), std::forward<Arg>(arg));
};

template<typename Func, typename ResultType>
concept ApplyNeedLValue = (CanInvoke<MySTD::RemoveReferenceType<Func>&, ResultType&> && 
                !CanInvoke<MySTD::RemoveReferenceType<Func>&, ResultType&&>);

template<typename ResultType>
class TTask
{
    TaskNodeResult<ResultType>* node_;
    TTaskScheduler* scheduler_;

public:
    TTask(TaskNodeResult<ResultType>* node, TTaskScheduler* scheduler) : node_(node), scheduler_(scheduler) {}

    template<typename T>
    decltype(auto) GetResultSync()
    {
        return TFuture<T>(node_).Get();
    }

    template<typename T>
    TFuture<T> GetFutureResult()
    {
        return TFuture<T>(node_);
    }

    template<typename Func>
        requires ApplyNeedLValue<Func, ResultType>
    auto Apply(Func&& func);

    template<typename Func>
        requires (!ApplyNeedLValue<Func, ResultType> &&
                    CanInvoke<MySTD::RemoveReferenceType<Func>&, ResultType&&>)
    auto Apply(Func&& func);
};