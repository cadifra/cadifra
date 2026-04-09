/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core:Base.IShiftable;
import :Base;


namespace Core
{

namespace
{
using C = IShiftable;
}


void C::shift(Env& e,
    IElement* sender,
    const ShiftVector& sv,
    const ShiftSet& dss,
    bool shallow)
{
    if (isInTrash())
        return;

    if (dss.isDeferredShifting(*this))
        return;

    shiftImpl(e, sender, sv, dss, shallow);
}


void C::detachFromSource(Env&,
    const ShiftSet&, const ElementSet& selection)
{
    // default: nothing to detach
}


void C::extendSelection(const ElementSet&, ElementSet&)
{
}

}
