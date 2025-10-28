/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;


namespace Core
{

namespace
{
using C = ExtendSelection::Param;
}


class C::CacheEntry
{
public:
    const IElement* previous;
    Result res;
    CacheEntry(const IElement* p, Result r):
        previous{ p }, res{ r }
    {
    }
};


C::Param(const ICaller& caller, const ElementSet& selection):
    itsCaller{ caller },
    itsSelection{ selection }
{
}


C::~Param() = default;


bool C::FindCacheEntry(const IElement* target, Result& res) const
{
    bool found = false;

    for (
        auto i = itsCache.find(target);
        i != end(itsCache) && i->first == target;
        ++i)
    {
        const auto& e = i->second;
        if (e.previous == 0)
        {
            res = e.res;
            return true;
        }
        if (e.previous == itsPrevious)
        {
            res = e.res;
            found = true;
        }
    }

    return found;
};


void C::EraseOtherCacheEntries(const Cache::iterator except)
{
    auto i = itsCache.find(except->first);

    while (i != end(itsCache) && i->first == except->first)
    {
        if (i == except)
            ++i;
        else
            i = itsCache.erase(i);
    }
}


auto C::Call(const IElement* target) -> Result
{
    D1_ASSERT(target);
    if (target->IsInTrash())
        return Result::no;

    auto res = Result::possibly;

    if (FindCacheEntry(target, res))
        return res;

    auto newEntry = itsCache.insert({ target, { 0, res } });
    // Repel recursion: this can happen with recursive data structures created by
    // the user (absolutely legal).
    // The default answer during recursion is "possibly".

    const auto* oldPrevious = itsPrevious;
    itsPrevious = target;
    res = itsCaller.MakeCall(target, *this);
    itsPrevious = oldPrevious;

    newEntry->second = { itsPrevious, res };

    if (!itsPrevious)
        EraseOtherCacheEntries(newEntry);

    return res;
}

}
