/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.algorithm;

import std;


export namespace d1
{

bool erase_first(auto& container, const auto& value)
//
//  Removes the first occurrence of value from container.
//  Works for std::vector.
//
{
    auto pos = std::ranges::find(container, value);
    if (pos != end(container))
    {
        container.erase(pos);
        return true;
    }
    return false;
}


bool erase_first_with_get(auto& container, auto& v)
{
    auto pos = std::ranges::find_if(container,
        [&](const auto& ele) {
            return (ele.get() == &v);
        });

    if (pos != end(container))
    {
        container.erase(pos);
        return true;
    }

    return false;
}


void push_back_all(auto& c1, const auto& c2)
{
    c1.insert(end(c1), cbegin(c2), cend(c2));
}


inline bool has_check(const auto& container, const auto& value)
{
    return std::ranges::find(container, value) != end(container);
}


inline void push_back_if_missing(auto& container, const auto& value)
{
    if (std::ranges::find(container, value) == end(container))
        container.push_back(value);
}


template <class It, class Comp>
inline It find_duplicate(It begin, It end, Comp& equals)
{
    for (auto i = begin; i != end; ++i)
    {
        for (auto j = std::next(i); j != end; ++j)
        {
            if (equals(*i, *j))
                return j;
        }
    }

    return end;
}


}
