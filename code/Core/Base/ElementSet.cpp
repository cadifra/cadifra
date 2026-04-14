/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;
import :Base;

import d1.algorithm;


namespace Core
{

namespace
{
using C = ElementSet;
}


bool C::insert(IElement& m)
{
    D1_ASSERT(&m != nullptr);

    if (contains(m))
        return false;

    auto sp = m.shared_from_this();
    auto ms = std::dynamic_pointer_cast<IElement>(sp);
    contents_.push_back(ms);
    return true;
}



void C::insert(const ElementSet& rhs)
{
    for (const auto& m : rhs)
    {
        D1_ASSERT(m.get() != nullptr);

        if (contains(*m.get()))
            continue;
        contents_.push_back(m);
    }
}



bool C::contains(const IElement& me) const
{
    D1_ASSERT(&me != nullptr);

    for (auto& i : contents_)
        if (i.get() == &me)
            return true;

    return false;
}


bool C::remove(IElement& me)
{
    D1_ASSERT(&me != nullptr);

    return d1::erase_first_with_get(contents_, me);
}



void C::print(std::ostream& os) const
{
#ifdef _DEBUG
    // printSortedIDs(os, begin(), end());
#endif
}


}
