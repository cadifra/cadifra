/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.Main:ElementSet;

import :IElement;

import std;

namespace Core
{

export class ElementSet
{
    std::vector<IElementRef> contents_;

public:
    ElementSet() {}
    ElementSet(const ElementSet& rhs) = default;

    template <class InIter>
    ElementSet(InIter a, InIter b)
    {
        for (; a != b; ++a)
        {
            if (not contains(*a->get()))
                insert(*a->get());
        }
    }

    auto clone() const { return *this; }

    bool insert(IElement&);
    void insert(const ElementSet&);

    bool remove(IElement&);
    bool contains(const IElement&) const;

    auto size() const
    {
        return static_cast<d1::int32>(contents_.size());
    }
    bool empty() const { return contents_.empty(); }
    void clear() { contents_.clear(); }
    auto begin() const { return contents_.begin(); }
    auto end() const { return contents_.end(); }


    void print(std::ostream&) const; // for debugging: writes all IDs

    friend std::ostream& operator<<(std::ostream& s, const ElementSet& d)
    {
        d.print(s);
        return s;
    }

    friend auto begin(const ElementSet& s)
    {
        return s.begin();
    }

    friend auto end(const ElementSet& s)
    {
        return s.end();
    }
};

export d1::nRect calcBoundingBox(const ElementSet&);

}

import d1.algorithm;

namespace Core
{

bool ElementSet::insert(IElement& m)
{
    D1_ASSERT(&m != nullptr);

    if (contains(m))
        return false;

    auto sp = m.shared_from_this();
    auto ms = std::dynamic_pointer_cast<IElement>(sp);
    contents_.push_back(ms);
    return true;
}



void ElementSet::insert(const ElementSet& rhs)
{
    for (const auto& m : rhs)
    {
        D1_ASSERT(m.get() != nullptr);

        if (contains(*m.get()))
            continue;
        contents_.push_back(m);
    }
}



bool ElementSet::contains(const IElement& me) const
{
    D1_ASSERT(&me != nullptr);

    for (auto& i : contents_)
        if (i.get() == &me)
            return true;

    return false;
}


bool ElementSet::remove(IElement& me)
{
    D1_ASSERT(&me != nullptr);

    return d1::erase_first_with_get(contents_, me);
}



void ElementSet::print(std::ostream& os) const
{
#ifdef _DEBUG
    // printSortedIDs(os, begin(), end());
#endif
}

}
