#pragma once
#include <coroutine>
#include <stdexcept>

class Player
{
public:
    struct promise_type;
    using handle = std::coroutine_handle<promise_type>;

    bool in_progress() { return coro and !coro.done(); }
    bool next_turn()
    {
        return in_progress() ? (coro.resume(), !coro.done()) : false;
    }

    int currentGuess() const
    {
        if (coro)
            return coro.promise().result;
        throw std::runtime_error("coroutine is destroyed");
    }
    operator bool() const noexcept { return bool(coro); }

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