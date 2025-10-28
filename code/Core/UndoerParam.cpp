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
using C = Undoer;
}


C::Param::Param(IDiagram& d, Selection::Tracker& sc):
    itsDiagram{ d },
    itsSelectionTracker{ sc }
{
}


C::Param::~Param()
{
    Finish();
}


void C::Param::Finish()
{
    for (auto me : itsAddToDiagram)
    {
        D1_ASSERT(!me->IsInTrash());
        itsDiagram.Insert(me);
        itsDiagram.CreateViewElements(*me);
    }
    itsAddToDiagram.clear();

    for (auto mi : itsRemoveFromDiagram)
    {
        IElement& me = *mi;
        D1_ASSERT(me.IsInTrash());
        itsDiagram.Remove(me);
        itsDiagram.DestroyViewElements(me, itsSelectionTracker);
    }
    itsRemoveFromDiagram.clear();

    for (auto mi : itsUpdateViews)
    {
        IElement& me = *mi;
        if (me.IsInTrash())
            continue;
        me.ViewsNeedUpdate(itsDiagram);
    }
    itsUpdateViews.clear();
}


void C::Param::AddToDiagram(const IElementRef& me)
{
    itsAddToDiagram.insert(me);
}


void C::Param::RemoveFromDiagram(const IElementRef& me)
{
    itsRemoveFromDiagram.insert(me);
}


void C::Param::UpdateViews(const IElementRef& me)
{
    itsUpdateViews.insert(me);
}

}
