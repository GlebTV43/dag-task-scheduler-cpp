#pragma once

#include <memory>
#include <utility>

#include "Exception.hpp"
#include "Tuple.hpp"
#include "IntergerSequence.hpp"
#include "SizeT.hpp"
#include "IsSame.hpp"
#include "Invoke.hpp"

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

public:

    template<typename T>
    void SetResult(T&& value) // сохраняем и copy и move
    {
        result_.reset(new ResultType(std::forward<T>(value)));
        result_is_moved_ = false;
    }

    ResultType& GetResultRef()
    {
        Execute();

        if (result_ == nullptr)
        {
            throw MySTD::EmptyResultException();
        }

        return *result_;
    }

    const ResultType& GetConstResultRef()
    {
        Execute();

        if (result_ == nullptr)
        {
            throw MySTD::EmptyResultException();
        }

        return *result_;
    }

    ResultType&& MoveResult()
    {
        Execute();

        if (result_ == nullptr)
        {
            throw MySTD::EmptyResultException();
        }

        if (result_is_moved_)
        {
            throw MySTD::ResultAlreadyMovedException();
        }

        result_is_moved_ = true;

        return std::move(*result_);
    }

    virtual ~TaskNodeResult() = default;
};

template<> // cспецаилизация для void 
class TaskNodeResult<void> : public TaskNodeBase
{
public:
    void SetResult() {}

    virtual ~TaskNodeResult() = default;
};
