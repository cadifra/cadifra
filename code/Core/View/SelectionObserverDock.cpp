/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;
import :View;

import d1.algorithm;


namespace Core
{

namespace
{
using C = ISelectionObserver;
}


void C::Dock::attach(ISelectionObserver& obs)
{
    observers_.push_back(&obs);
    obs.setDock(this);
}


void C::Dock::detach(ISelectionObserver& obs)
{
    d1::erase_first(observers_, &obs);
    obs.setDock(nullptr);
}


void C::Dock::notify() const
{
    for (auto* obs : observers_)
    {
        D1_ASSERT(obs);
        obs->selectionChanged();
    }
}

}
