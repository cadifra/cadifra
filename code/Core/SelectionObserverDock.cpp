/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

import d1.algorithm;


namespace Core
{

namespace
{
using C = ISelectionObserver;
}


void C::Dock::Attach(ISelectionObserver& obs)
{
    itsObservers.push_back(&obs);
    obs.SetDock(this);
}


void C::Dock::Detach(ISelectionObserver& obs)
{
    d1::erase_first(itsObservers, &obs);
    obs.SetDock(0);
}


void C::Dock::Notify() const
{
    for (auto* obs : itsObservers)
    {
        D1_ASSERT(obs);
        obs->SelectionChanged();
    }
}

}
