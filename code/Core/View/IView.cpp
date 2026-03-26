/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core:View;

import WinUtil.Debug;


namespace Core
{

using std::ranges::for_each;

namespace
{
using C = IView;
}


C::IView(IClub& c):
    club_{ c }
{
}


void C::changeActiveState(bool active)
{
    if (deleting_)
        return;

    if (active == active_)
        return; // no change

    active_ = active;
}


C::~IView()
{
    deleting_ = true;

    auto a = [&](auto* ve) { ve->viewDeleted(); };
    for_each(viewElements_, a);
}


void C::insert(IElement& me, bool update_view)
{
    if (deleting_)
        return;

    IViewElement* existing_ve = me.viewElement(this);

    if (existing_ve)
        return; // me already has a view element in this view

    D1_ASSERT(not existing_ve);

    IViewElement* new_ve = me.makeViewElement(*this);
    if (new_ve)
        new_ve->insert(update_view);
}


#ifdef _DEBUG
namespace
{
d1::uint32 gDebugFindPointableAmbiguous = 0;
bool findPointableDebugEnabled()
{
    return (WinUtil::DebugEnv::inst().getInt("Core", "IView::DebugFindPointable") != 0);
}
}
#endif


auto C::findPointable(const d1::Point& p, d1::int32 unzoomed_distance,
    const IFilter* theFilter, bool attaching) const -> VIPointable*
{
    if (deleting_)
        return nullptr;

    d1::int32 distance = this->zoomedDistance(unzoomed_distance);
    Weight max;
    bool first = true;
    VIPointable* res = nullptr;
    IViewElement* max_ve = nullptr;
    int num_candidates = 0;

    for (auto ve : viewElements_)
    {
        D1_ASSERT(ve);

        if (theFilter and not theFilter->pass(*ve))
            continue;

        if (not attaching)
        {
            auto sve = ve->selectable();
            if (sve and sve->isSelected())
            {
                VIPointable* pb = sve->findControl(p, distance);
                if (pb)
                {
                    auto lev = pb->getWeight(p, distance);
                    if (first or (lev > max))
                    {
                        first = false;
                        max = lev;
                        res = pb;
                        max_ve = ve;
                        num_candidates = 1;
                    }
                    else if (lev == max)
                        ++num_candidates;
                }
            }
        }

        if (ve->checkHit(p, distance, attaching))
        {
            auto lev = ve->getWeight(p, distance);
            if (first or lev > max)
            {
                first = false;
                max = lev;
                res = ve;
                max_ve = ve;
                num_candidates = 1;
            }
            else if (lev == max)
                ++num_candidates;
        }
    } // for

#ifdef _DEBUG
    if (findPointableDebugEnabled() and num_candidates > 1)
    {
        ++gDebugFindPointableAmbiguous;
        WinUtil::dout << "@@@ Core::IView::FindPointable ambiguous: " << num_candidates
                      << " candidates"
                      << " (" << static_cast<unsigned long>(gDebugFindPointableAmbiguous)
                      << ". time)" << std::endl;
    }
#endif

    return res;
}


bool C::hasSelection() const
{
    if (deleting_)
        return false;

    for (auto ve : viewElements_)
    {
        D1_ASSERT(ve);
        if (ve->isSelected())
            return true;
    }
    return false; // nothing selected
}


void C::deselectAll(Selection::Tracker& sc)
{
    if (deleting_)
        return;

    for (auto ve : viewElements_)
    {
        D1_ASSERT(ve);
        auto* sve = ve->selectable();
        if (sve)
            sve->deselect(sc);
    }
}


void C::selectAll(Selection::Tracker& sc)
{
    if (deleting_)
        return;

    for (auto ve : viewElements_)
    {
        D1_ASSERT(ve);
        auto* sve = ve->selectable();
        if (sve)
            sve->select(sc);
    }
}


auto C::selection() const -> ElementSet
{
    ElementSet res;

    if (deleting_)
        return res;

    for (auto ve : viewElements_)
    {
        D1_ASSERT(ve);
        if (ve->isSelected())
            res.insert(ve->element());
    }
    return res;
}


void C::setSelectionVisibility(bool visible)
{
    if (deleting_)
        return;

    if (selectionVisible_ == visible)
        return;

    selectionVisible_ = visible;

    for (auto ve : viewElements_)
    {
        D1_ASSERT(ve);
        auto* sve = ve->selectable();
        if (sve and sve->isSelected())
            sve->selectionNeedsUpdate();
    }
}


void C::notifySelectionObservers() const
{
    if (deleting_)
        return;
    selectionObserverDock_.notify();
}



void C::setSelection(Selection::Tracker& sc, ElementSet theSelection)
{
    if (deleting_)
        return;

    for (auto ve : viewElements_)
    {
        D1_ASSERT(ve);
        bool selected = theSelection.contains(ve->element());
        auto* sve = ve->selectable();
        if (sve)
            sve->setSelectionState(sc, selected);
    }
}


auto C::hideSelection() -> Selection::Hider
{
    return selectionVisibilityServer_.hideSelection();
}


bool C::has(IViewElement& v) const
{
    return viewElements_.find(&v) != viewElements_.end();
}


void C::add(IViewElement& ve)
{
    if (deleting_)
        return;

    viewElements_.insert(&ve);
    // inserts only if ve is not yet contained.

#ifdef _DEBUG
    // some consistency checks
    {
        IElement& me = ve.element();
        D1_ASSERT(&me);

        IView& view_of_ve = ve.getView();
        D1_ASSERT(&view_of_ve == this);

        IViewElement* ve_of_me = me.viewElement(this);
        D1_ASSERT(ve_of_me);
        D1_ASSERT(ve_of_me == &ve);
    }
#endif
}


void C::add(VISelectable& ve)
{
    if (deleting_)
        return;

    viewElements_.insert(&ve);
    // inserts only if ve is not yet contained.

#ifdef _DEBUG
    // some consistency checks
    {
        IElement& me = ve.element();
        D1_ASSERT(&me);

        IView& view_of_ve = ve.getView();
        D1_ASSERT(&view_of_ve == this);

        IViewElement* ve_of_me = me.viewElement(this);
        D1_ASSERT(ve_of_me);
        D1_ASSERT(ve_of_me == &ve);
    }
#endif
}


void C::remove(IViewElement& ve)
{
    if (deleting_)
        return;

    viewElements_.erase(&ve);
}


void C::remove(VISelectable& ve)
{
    if (deleting_)
        return;

    viewElements_.erase(&ve);
}

}
