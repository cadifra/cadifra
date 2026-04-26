/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core.Shift;


namespace Core
{

namespace
{
using C = ShiftSet;
}


C::ShiftSet()
{
    actualShiftVector_.shiftX = false;
    actualShiftVector_.shiftY = false;
}


C::ShiftSet(
    const ElementSet& selection,
    IDiagram& d,
    IShiftable* theMasterMover):

    masterMover_{ theMasterMover }
{
    actualShiftVector_.shiftX = false;
    actualShiftVector_.shiftY = false;

    for (auto me : d)
    {
        D1_ASSERT(me);
        if (auto s = dynamic_cast<IShiftable*>(me))
            s->addNormal(*this, selection);
    }

    for (auto ds : deferredShiftables_)
    {
        actualSender_ = ds;
        actualSender_->addDependents(*this);
    }

    for (auto me : selection)
    {
        D1_ASSERT(me);
        auto s = dynamic_cast<IShiftable*>(me.get());
        if (s and not isDeferredShifting(*s))
            instantShiftables_.insert(s);
    }

    actualSender_ = 0;
}


void C::deferredShift(
    Env& e, const ShiftVector& sv, const d1::fPoint& mouse_pos)
{
    actualShiftVector_ = sv;
    actualMousePos_ = mouse_pos;

    for (auto is : instantShiftables_)
        is->shift(e, 0, sv, *this, false /*not shallow*/);

    for (auto& d : dependents_)
        d.second->shiftImpl(e, d.first, sv, *this, false /*not shallow*/);
}


void C::finalShift(Env& e)
{
    for (auto s : deferredShiftables_)
        s->shiftImpl(e, 0, actualShiftVector_, *this, true /*shallow*/);
}


auto C::getDeferredShift() const -> const ShiftVector&
{
    return actualShiftVector_;
}


void C::addNormal(IShiftable& s)
{
    deferredShiftables_.insert(&s);
}


void C::addDependent(IShiftable& s)
{
    D1_ASSERT(&s);
    if (not isDeferredShifting(s))
        dependents_.insert(std::pair{ actualSender_, &s });
}


bool C::isDeferredShifting(const IShiftable& s) const
{
    return deferredShiftables_.find(const_cast<IShiftable*>(&s)) != deferredShiftables_.end();
}


void C::print(std::ostream& os) const
{
#ifdef _DEBUG
    Core::printSortedIDs(os, deferredShiftables_);
#endif
}


}
