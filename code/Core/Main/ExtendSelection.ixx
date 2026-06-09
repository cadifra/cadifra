/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:ExtendSelection;

import std;


namespace Core
{

export {
class IElement;
class ElementSet;
}


namespace ExtendSelection
{

export enum class Result {
    no = 0,
    possibly,
    yes
};


export class Param
{
public:
    class ICaller;

    Param(const ICaller& caller, const ElementSet& selection);
    ~Param();

    Param(const Param&) = delete;
    Param& operator=(const Param&) = delete;

    auto call(const IElement* target) -> Result;

    auto selection() const -> const ElementSet& { return selection_; }

private:
    struct CacheEntry;
    using Cache = std::multimap<const IElement*, CacheEntry>;

    bool findCacheEntry(const IElement* target, Result& res) const;

    void eraseOtherCacheEntries(const Cache::iterator except);

    Cache cache_;
    const ICaller& caller_;
    const ElementSet& selection_;
    const IElement* previous_ = nullptr;
};


class Param::ICaller
{
public:
    virtual Result makeCall(const IElement* target, Param& p) const = 0;

protected:
    ~ICaller() = default;
};

}

}
