/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

namespace
{
using C = IShiftable;
}


void C::Shift(Env& e,
    IElement* sender,
    const ShiftVector& sv,
    const DeferredShiftSet& dss,
    bool shallow)
{
    if (IsInTrash())
        return;

    if (dss.IsDeferredShifting(*this))
        return;

    ShiftImpl(e, sender, sv, dss, shallow);
}


void C::DetachFromSource(Env&,
    const DeferredShiftSet&, const ElementSet& selection)
{
    // default: nothing to detach
}


void C::ExtendSelection(const ElementSet&, ElementSet&)
{
}

}
