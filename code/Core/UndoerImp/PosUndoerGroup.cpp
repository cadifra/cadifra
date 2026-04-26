/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core.UndoerImp;

import d1.algorithm;


namespace Core
{

namespace
{
using C = PosUndoerGroup;
}


void C::undoImp(Param& p)
{
    const auto delta = d1::Vector{ -offset_.dx, -offset_.dy };

    for (auto o : objects_)
    {
        o->move(delta);
        p.updateViews({ o });
    }
}


void C::redoImp(Param& p)
{
    for (auto o : objects_)
    {
        o->move(offset_);
        p.updateViews({ o });
    }
}


bool C::merge(Undoer& u)
{
    return false;
}


void C::remove(IElement& me)
{
    d1::erase_first_with_get(objects_, me);
}


bool C::isNull() const
{
    return objects_.empty();
}


C::PosUndoerGroup(const d1::Vector& offset):
    offset_{ offset }
{
}


void C::add(IPosOwner& po)
{
    D1_ASSERT(&po);

    auto sp = po.shared_from_this();
    auto spo = std::dynamic_pointer_cast<IPosOwner>(sp);

    objects_.push_back(spo);
}

}
