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
using C = ShiftSet;
}


C::ShiftSet()
{
    itsActualShiftVector.shiftX = false;
    itsActualShiftVector.shiftY = false;
}


C::ShiftSet(
    const ElementSet& selection,
    IDiagram& d,
    IShiftable* theMasterMover):

    itsMasterMover{ theMasterMover }
{
    itsActualShiftVector.shiftX = false;
    itsActualShiftVector.shiftY = false;

    for (auto me : d)
    {
        D1_ASSERT(me);
        if (auto s = dynamic_cast<IShiftable*>(me))
            s->AddNormal(*this, selection);
    }

    for (auto ds : itsDeferredShiftables)
    {
        itsActualSender = ds;
        itsActualSender->AddDependents(*this);
    }

    for (auto me : selection)
    {
        D1_ASSERT(me);
        auto s = dynamic_cast<IShiftable*>(me.get());
        if (s && !IsDeferredShifting(*s))
            itsInstantShiftables.insert(s);
    }

    itsActualSender = 0;
}


void C::DeferredShift(
    Env& e, const ShiftVector& sv, const d1::fPoint& mouse_pos)
{
    itsActualShiftVector = sv;
    itsActualMousePos = mouse_pos;

    for (auto is : itsInstantShiftables)
        is->Shift(e, 0, sv, *this, false /*not shallow*/);

    for (auto& d : itsDependents)
        d.second->ShiftImpl(e, d.first, sv, *this, false /*not shallow*/);
}


void C::FinalShift(Env& e)
{
    for (auto s : itsDeferredShiftables)
        s->ShiftImpl(e, 0, itsActualShiftVector, *this, true /*shallow*/);
}


auto C::GetDeferredShift() const -> const ShiftVector&
{
    return itsActualShiftVector;
}


void C::AddNormal(IShiftable& s)
{
    itsDeferredShiftables.insert(&s);
}


void C::AddDependent(IShiftable& s)
{
    D1_ASSERT(&s);
    if (!IsDeferredShifting(s))
        itsDependents.insert(std::make_pair(itsActualSender, &s));
}


bool C::IsDeferredShifting(const IShiftable& s) const
{
    return itsDeferredShiftables.find(const_cast<IShiftable*>(&s)) != itsDeferredShiftables.end();
}


void C::Print(std::ostream& os) const
{
#ifdef _DEBUG
    Core::PrintSortedIDs(os, itsDeferredShiftables);
#endif
}


}
