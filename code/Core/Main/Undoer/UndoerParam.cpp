/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core.Main;


namespace Core
{

namespace
{
using C = Undoer;
}


C::Param::Param(IDiagram& d, Selection::Tracker& sc):
    diagram_{ d },
    selectionTracker_{ sc }
{
}


C::Param::~Param()
{
    finish();
}


void C::Param::finish()
{
    for (auto me : addToDiagram_)
    {
        D1_ASSERT(not me->isInTrash());
        diagram_.insert(me);
        diagram_.createViewElements(*me);
    }
    addToDiagram_.clear();

    for (auto mi : removeFromDiagram_)
    {
        IElement& me = *mi;
        D1_ASSERT(me.isInTrash());
        diagram_.remove(me);
        diagram_.destroyViewElements(me, selectionTracker_);
    }
    removeFromDiagram_.clear();

    for (auto mi : updateViews_)
    {
        IElement& me = *mi;
        if (me.isInTrash())
            continue;
        me.viewsNeedUpdate(diagram_);
    }
    updateViews_.clear();
}


void C::Param::addToDiagram(const IElementRef& me)
{
    addToDiagram_.insert(me);
}


void C::Param::removeFromDiagram(const IElementRef& me)
{
    removeFromDiagram_.insert(me);
}


void C::Param::updateViews(const IElementRef& me)
{
    updateViews_.insert(me);
}

}
