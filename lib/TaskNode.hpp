#pragma once

#include <memory>
#include <utility>

#include "Exception.hpp"
#include "Tuple.hpp"
#include "IntergerSequence.hpp"
#include "SizeT.hpp"

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

template<typename ResultType>
class TaskNodeResult : public TaskNodeBase // промежуточный класс, тк TTask и TFuture не должны знать Func и Args...
{
protected:

    std::unique_ptr<ResultType> result_;
    bool result_is_moved_ = false;

    template<typename T>
    void SetResult(T&& value) // сохраняем и copy и move
    {
        result_.reset(new ResultType(std::forward<T>(value)));
        result_is_moved_ = false;
    }

public:

    ResultType& GetResultRef()
    {
        Execute();

        if (result_ == nullptr)
        {
            throw EmptyResultException();
        }

        return *result_;
    }

    const ResultType& GetConstResultRef()
    {
        Execute();

        if (result_ == nullptr)
        {
            throw EmptyResultException();
        }

        return *result_;
    }

    ResultType&& MoveResult()
    {
        Execute();

        if (result_ == nullptr)
        {
            throw EmptyResultException();
        }

        if (result_is_moved_)
        {
            throw ResultAlreadyMovedException();
        }

        result_is_moved_ = true;

        return std::move(*result_);
    }

    virtual ~TaskNodeResult() = default;
};

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
    
    T Get()
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
    
    T Get()
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
        this->SetResult(Invoke(func_, Resolve(Get<I>(args_))...));
    }


public:
    template<typename AnotherFunc, typename... AArgs> 
    TaskNode(AnotherFunc&& func, AArgs&&... args) : func_(std::move(func)), args_(std::forward<AArgs>(args)...) {}

    void Execute() override
    {
        if (this->state_ == Status::kFinished)
        {
            return;
        }
        else if (this->state_ == Status::kExecuting)
        {
            throw DependencyCycleException();
        }

        this->state_ = Status::kExecuting;

        ExecuteImpl(MySTD::MakeIndexSequence<sizeof...(Args)>{});

        this->state_ = Status::kFinished;
    }
};

template<typename ResultType>
class TTask
{
    TaskNodeResult<ResultType>* node_;

public:
    explicit TTask(TaskNodeResult<ResultType>* node) : node_(node) {}

    template<typename T>
    T GetResultSync()
    {
        return TFuture<T>(node_).Get();
    }

    template<typename T>
    TFuture<T> GetFutureResult()
    {
        return TFuture<T>(node_);
    }
};