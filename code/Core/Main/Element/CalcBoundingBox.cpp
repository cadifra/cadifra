/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core.Main:Element;

import Core.Shift;

import d1.Rect;


namespace Core
{

d1::nRect calcBoundingBox(const ElementSet& mes)
{
    d1::nRect max;

    for (auto& ms : mes)
    {
        auto* me = ms.get();
        D1_ASSERT(me);

        if (auto sme = dynamic_cast<IShiftable*>(me))
        {
            if (auto obox = sme->getBoundingBox())
            {
                auto box = obox.value();
                D1_ASSERT(box.isNormalized());
                max += box;
            }
        }
    }
    return max;
}

}
