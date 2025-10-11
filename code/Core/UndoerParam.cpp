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
using C = UndoerParam;
}


C::UndoerParam(IDiagram& d, SelectionTracker& sc):
    itsDiagram{ d },
    itsSelectionTracker{ sc }
{
}


C::~UndoerParam()
{
    Finish();
}


void C::Finish()
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


void C::AddToDiagram(const IElementPtr& me)
{
    itsAddToDiagram.insert(me);
}


void C::RemoveFromDiagram(const IElementPtr& me)
{
    itsRemoveFromDiagram.insert(me);
}


void C::UpdateViews(const IElementPtr& me)
{
    itsUpdateViews.insert(me);
}

}
