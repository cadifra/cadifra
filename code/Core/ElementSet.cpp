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
using C = ElementSet;
}


bool C::Insert(IElement& m)
{
    D1_ASSERT(&m != nullptr);

    if (Contains(m))
        return false;

    auto sp = m.shared_from_this();
    auto ms = std::dynamic_pointer_cast<IElement>(sp);
    itsContents.push_back(ms);
    return true;
}



void C::Insert(const ElementSet& rhs)
{
    for (const auto& m : rhs)
    {
        D1_ASSERT(m.get() != nullptr);

        if (Contains(*m.get()))
            continue;
        itsContents.push_back(m);
    }
}



bool C::Contains(const IElement& me) const
{
    D1_ASSERT(&me != nullptr);

    for (auto& i : itsContents)
        if (i.get() == &me)
            return true;

    return false;
}


bool C::Remove(IElement& me)
{
    D1_ASSERT(&me != nullptr);

    for (auto i = itsContents.begin();
        i != itsContents.end();
        ++i)
    {
        if (i->get() == &me)
        {
            itsContents.erase(i);
            return true;
        }
    }

    return false;
}



void C::Print(std::ostream& os) const
{
#ifdef _DEBUG
    // PrintSortedIDs(os, begin(), end());
#endif
}


}
