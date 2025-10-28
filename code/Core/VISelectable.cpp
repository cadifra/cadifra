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
using C = VISelectable;
}


void C::Insert(bool needsUpdate)
{
    GetView().Add(*this);

    if (needsUpdate)
        NeedsUpdate();
}


void C::SetSelectionState(Selection::Tracker& st, bool newstate)
{
    bool change = itIsSelected != newstate;

    if (change)
    {
        itIsSelected = newstate;
        st.Changed();
    }

    auto& me = Element();
    D1_ASSERT(&me);

    if (change)
        SelectionNeedsUpdate();

#ifdef _DEBUG
    // some consistency checks
    {
        IView& view_of_this = GetView();
        IViewElement* ve_of_me = me.ViewElement(&view_of_this);
        D1_ASSERT(ve_of_me);
        D1_ASSERT(ve_of_me == this);
    }
#endif
}


bool C::SelectionIsVisible() const
{
    return GetView().SelectionIsVisible();
}


auto C::FindControl(const d1::Point& p, d1::int32 distance) -> VIPointable*
{
    return nullptr;
}

}
