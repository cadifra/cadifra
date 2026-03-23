/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core:View;


namespace Core
{

namespace
{
using C = VISelectable;
}


void C::insert(bool needs_update)
{
    getView().add(*this);

    if (needs_update)
        needsUpdate();
}


void C::setSelectionState(Selection::Tracker& st, bool newstate)
{
    bool change = selected_ != newstate;

    if (change)
    {
        selected_ = newstate;
        st.changed();
    }

    auto& me = element();
    D1_ASSERT(&me);

    if (change)
        selectionNeedsUpdate();

#ifdef _DEBUG
    // some consistency checks
    {
        IView& view_of_this = getView();
        IViewElement* ve_of_me = me.viewElement(&view_of_this);
        D1_ASSERT(ve_of_me);
        D1_ASSERT(ve_of_me == this);
    }
#endif
}


bool C::selectionIsVisible() const
{
    return getView().selectionIsVisible();
}


auto C::findControl(const d1::Point& p, d1::int32 distance) -> VIPointable*
{
    return nullptr;
}

}
