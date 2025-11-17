/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

import d1.Point;


namespace Core
{

namespace
{
using C = IViewElement;
}


bool C::CheckHit(const d1::Point& pos, d1::int32 dist, bool attaching) const
{
    return Element().CheckHit(pos, dist, attaching);
}


C::~IViewElement()
{
    itsView.Remove(*this);
}


void C::Insert(bool needsUpdate)
{
    itsView.Add(*this);

    D1_ASSERT(&itsView.Club() == Element().Club());

    if (needsUpdate)
        NeedsUpdate();
}


void C::ViewDeleted()
{
    auto& e = Element();
    e.Delete(*this);
}


bool C::IsInActiveView() const { return itsView.IsActive(); }


auto C::GetWeight(const d1::Point& pos, d1::int32 distance) const
    -> Weight
{
    return Element().GetWeight(&itsView, pos, distance);
}


bool C::IsSelected() const
{
    auto* sve = Selectable();
    return sve && sve->IsSelected();
}

}
