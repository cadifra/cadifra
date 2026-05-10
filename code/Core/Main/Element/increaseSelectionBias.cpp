/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core.Main;

import d1.isEqual;

import GraphUtil.Shapes;


namespace Core
{

void increaseSelectionBias(Weight& w, const IView* v, const IElement& m)
{
    if (m.isSelected(v))
        w.increaseSelectionBias();
}

}
