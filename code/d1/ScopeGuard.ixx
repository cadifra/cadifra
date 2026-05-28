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
        if (not *this)
            return;
        try
        {
            std::invoke(*this);
        }
        catch (...)
        {
        }
    }

    void dismiss(this ScopeGuard& self)
    {
        self = ScopeGuard{};
    }
};

// usage:
//
// auto guard1 = ScopeGuard{ [] { std::cout << "hello"; } };
//
// auto guard2 = ScopeGuard{ [=] { deleteFun(ptr); } };
//
// auto guard3 = ScopeGuard{ [&] { eat(bread); } };
//
// auto guard4 = ScopeGuard{ std::bind(deleteFun, ptr) };

}
