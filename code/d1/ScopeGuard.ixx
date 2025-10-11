/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.ScopeGuard;

import std;

namespace d1
{

export struct ScopeGuard: std::function<void()>
{
    ~ScopeGuard() noexcept
    {
        if (!*this)
            return;
        try
        {
            std::invoke(*this);
        }
        catch (...)
        {
        }
    }

    void Dismiss(this ScopeGuard& self)
    {
        self = ScopeGuard{};
    }
};

// usage:
//
// ScopeGuard guard1{ [] { std::cout << "hello"; } };
//
// ScopeGuard guard2{ [=] { deleteFun(ptr); } };
//
// ScopeGuard guard3{ [&] { eat(bread); } };
//
// ScopeGuard guard4{ std::bind(deleteFun, ptr) };


}
