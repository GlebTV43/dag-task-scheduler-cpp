#pragma once

namespace MySTD
{

class Exception
{
    const char* message_;
public: 
    explicit Exception(const char* message) noexcept : message_(message) {}

    Exception(const Exception& other) noexcept = default;
    Exception& operator=(const Exception& other) noexcept = default;

    Exception(Exception&& other) noexcept = default;
    Exception& operator=(Exception&& other) noexcept = default;

    virtual ~Exception() noexcept = default;

    virtual const char* What() const noexcept
    {
        return message_;
    }
};

class TaskNotFoundException final : public Exception {
public:
    TaskNotFoundException() noexcept : Exception("Task not found") {}
};

class DependencyCycleException final : public Exception {
public:
    DependencyCycleException() noexcept : Exception("Dependency cycle detected") {}
};

class InvalidTaskException final : public Exception {
public:
    InvalidTaskException() noexcept : Exception("Invalid task") {}
};

class InvalidDependencyException final : public Exception {
public:
    InvalidDependencyException() noexcept : Exception("Invalid dependency") {}
};

class EmptyResultException final : public Exception {
public:
    EmptyResultException() noexcept : Exception("EmptyResult") {}
};

class ResultAlreadyMovedException final : public Exception {
public:
    ResultAlreadyMovedException() noexcept : Exception("Result has been already moved") {}
};

} // namespace MySTD