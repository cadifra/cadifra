/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;
import :Base;


namespace Core
{

namespace
{
using C = ExtendSelection::Param;
}


struct C::CacheEntry
{
    const IElement* previous;
    Result res;
    CacheEntry(const IElement* p, Result r):
        previous{ p }, res{ r }
    {
    }
};


C::Param(const ICaller& caller, const ElementSet& selection):
    caller_{ caller },
    selection_{ selection }
{
}


C::~Param() = default;


bool C::findCacheEntry(const IElement* target, Result& res) const
{
    bool found = false;

    for (
        auto i = cache_.find(target);
        i != end(cache_) and i->first == target;
        ++i)
    {
        const auto& e = i->second;
        if (e.previous == 0)
        {
            res = e.res;
            return true;
        }
        if (e.previous == previous_)
        {
            res = e.res;
            found = true;
        }
    }

    return found;
};


void C::eraseOtherCacheEntries(const Cache::iterator except)
{
    auto i = cache_.find(except->first);

    while (i != end(cache_) and i->first == except->first)
    {
        if (i == except)
            ++i;
        else
            i = cache_.erase(i);
    }
}


auto C::call(const IElement* target) -> Result
{
    D1_ASSERT(target);
    if (target->isInTrash())
        return Result::no;

    auto res = Result::possibly;

    if (findCacheEntry(target, res))
        return res;

    auto newEntry = cache_.insert({ target, { 0, res } });
    // Repel recursion: this can happen with recursive data structures created by
    // the user (absolutely legal).
    // The default answer during recursion is "possibly".

    const auto* oldPrevious = previous_;
    previous_ = target;
    res = caller_.makeCall(target, *this);
    previous_ = oldPrevious;

    newEntry->second = { previous_, res };

    if (not previous_)
        eraseOtherCacheEntries(newEntry);

    return res;
}

}
