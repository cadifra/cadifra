/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core:View;

import d1.Point;


namespace Core
{

namespace
{
using C = IViewElement;
}


bool C::checkHit(const d1::Point& pos, d1::int32 dist, bool attaching) const
{
    return element().checkHit(pos, dist, attaching);
}


C::~IViewElement()
{
    view_.remove(*this);
}


void C::insert(bool needs_update)
{
    view_.add(*this);

    D1_ASSERT(&view_.club() == element().club());

    if (needs_update)
        needsUpdate();
}


void C::viewDeleted()
{
    auto& e = element();
    e.Delete(*this);
}


bool C::isInActiveView() const { return view_.isActive(); }


auto C::getWeight(const d1::Point& pos, d1::int32 distance) const
    -> Weight
{
    return element().getWeight(&view_, pos, distance);
}


bool C::isSelected() const
{
    auto* sve = selectable();
    return sve and sve->isSelected();
}

}
