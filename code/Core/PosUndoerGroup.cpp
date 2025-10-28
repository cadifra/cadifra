/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

import d1.algorithm;


namespace Core
{

namespace
{
using C = PosUndoerGroup;
}


void C::UndoImp(Param& p)
{
    const auto delta = d1::Vector{ -itsOffset.dx, -itsOffset.dy };

    for (auto o : itsObjects)
    {
        o->Move(delta);
        p.UpdateViews({ o });
    }
}


void C::RedoImp(Param& p)
{
    for (auto o : itsObjects)
    {
        o->Move(itsOffset);
        p.UpdateViews({ o });
    }
}


bool C::Merge(Undoer& u)
{
    return false;
}


void C::Remove(IElement& me)
{
    d1::erase_first_with_get(itsObjects, me);
}


bool C::IsNull() const
{
    return itsObjects.empty();
}


C::PosUndoerGroup(const d1::Vector& offset):
    itsOffset{ offset }
{
}


void C::Add(IPosOwner& po)
{
    D1_ASSERT(&po);

    auto sp = po.shared_from_this();
    auto spo = std::dynamic_pointer_cast<IPosOwner>(sp);

    itsObjects.push_back(spo);
}

}
