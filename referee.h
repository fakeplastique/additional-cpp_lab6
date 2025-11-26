#pragma once
#include <coroutine>
#include <exception>
#include <memory>


class Referee
{
public:
    struct promise_type;
    using handle = std::coroutine_handle<promise_type>;
    int guess(int question)
    {
        *coro.promise().question = question;
        coro.resume();
        return coro.promise().answer;
    }

    struct promise_type
    {
        int answer = 0;
        std::shared_ptr<int> question;
        int number;
        promise_type(int number, std::shared_ptr<int> question)
            : question(question), number(number) {}
        void unhandled_exception() { std::terminate(); }
        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        auto yield_value(int value)
        {
            answer = value;
            return std::suspend_always{};
        }
        auto get_return_object()
        {
            return Referee{handle::from_promise(*this)};
        }
        void return_void(){}
    };

    Referee(Referee const &) = delete;
    Referee(Referee &&other) : coro(other.coro) { other.coro = nullptr; }
    ~Referee()
    {
        if (coro)
            coro.destroy();
    }

private:
    Referee(handle h) : coro(h) {}
    handle coro;
};