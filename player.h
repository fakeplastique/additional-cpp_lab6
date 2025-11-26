#pragma once
#include <coroutine>
#include <stdexcept>

class Player
{
public:
    struct promise_type;
    using handle = std::coroutine_handle<promise_type>;
    int operator()()
    {
        coro.resume();
        return coro.promise().result;
    }

    struct promise_type
    {
        int result = 0;
        void unhandled_exception() { std::terminate(); }
        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        auto yield_value(int value)
        {
            result = value;
            return std::suspend_always{};
        }
        auto get_return_object()
        {
            return Player{handle::from_promise(*this)};
        }
        void return_void(){}
    };

    Player(Player const &) = delete;
    Player &operator=(Player const &) = delete;
    Player(Player &&other) : coro(other.coro) { other.coro = nullptr; }
    ~Player()
    {
        if (coro)
            coro.destroy();
    }

private:
    Player(handle h) : coro(h) {}
    handle coro;
};
