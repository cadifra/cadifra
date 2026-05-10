/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core.Main:Selection;

import :Diagram;

import Core.Shift;


namespace Core
{

auto extendedSelection(const ElementSet& selection, IDiagram& d) -> ElementSet
{
    auto res = selection.clone();

    for (auto* me : d)
    {
        D1_ASSERT(me);

        if (auto s = dynamic_cast<IShiftable*>(me))
            s->extendSelection(selection, res);
    }

    return res;
}

}
