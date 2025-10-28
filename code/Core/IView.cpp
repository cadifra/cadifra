/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

import WinUtil.Debug;


namespace Core
{

using std::ranges::for_each;

namespace
{
using C = IView;
}


C::IView(IClub& c):
    itsClub{ c }
{
}


void C::ChangeActiveState(bool isActive)
{
    if (itsDeleting)
        return;

    if (isActive == itIsActive)
        return; // no change

    itIsActive = isActive;
}


C::~IView()
{
    itsDeleting = true;

    auto a = [&](auto* ve) { ve->ViewDeleted(); };
    for_each(itsViewElements, a);
}


void C::Insert(IElement& me, bool update_view)
{
    if (itsDeleting)
        return;

    IViewElement* existing_ve = me.ViewElement(this);

    if (existing_ve)
        return; // me already has a view element in this view

    D1_ASSERT(!existing_ve);

    IViewElement* new_ve = me.MakeViewElement(*this);
    if (new_ve)
        new_ve->Insert(update_view);
}


#ifdef _DEBUG
namespace
{
d1::uint32 gDebugFindPointableAmbiguous = 0;
bool FindPointableDebugEnabled()
{
    return (WinUtil::DebugEnv::Inst().GetInt("Core", "IView::DebugFindPointable") != 0);
}
}
#endif


auto C::FindPointable(const d1::Point& p, d1::int32 unzoomed_distance,
    const IFilter* theFilter, bool attaching) const -> VIPointable*
{
    if (itsDeleting)
        return nullptr;

    d1::int32 distance = this->ZoomedDistance(unzoomed_distance);
    Weight max;
    bool first = true;
    VIPointable* res = 0;
    IViewElement* max_ve = 0;
    int num_candidates = 0;

    for (auto ve : itsViewElements)
    {
        D1_ASSERT(ve);

        if (theFilter && !theFilter->Pass(*ve))
            continue;

        if (!attaching)
        {
            auto sve = ve->Selectable();
            if (sve && sve->IsSelected())
            {
                VIPointable* pb = sve->FindControl(p, distance);
                if (pb)
                {
                    auto lev = pb->GetWeight(p, distance);
                    if (first || (lev > max))
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

        if (ve->CheckHit(p, distance, attaching))
        {
            auto lev = ve->GetWeight(p, distance);
            if (first || lev > max)
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
    if (FindPointableDebugEnabled() && num_candidates > 1)
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


bool C::HasSelection() const
{
    if (itsDeleting)
        return false;

    for (auto ve : itsViewElements)
    {
        D1_ASSERT(ve);
        if (ve->IsSelected())
            return true;
    }
    return false; // nothing selected
}


void C::DeselectAll(Selection::Tracker& sc)
{
    if (itsDeleting)
        return;

    for (auto ve : itsViewElements)
    {
        D1_ASSERT(ve);
        auto* sve = ve->Selectable();
        if (sve)
            sve->Deselect(sc);
    }
}


void C::SelectAll(Selection::Tracker& sc)
{
    if (itsDeleting)
        return;

    for (auto ve : itsViewElements)
    {
        D1_ASSERT(ve);
        auto* sve = ve->Selectable();
        if (sve)
            sve->Select(sc);
    }
}


auto C::Selection() const -> ElementSet
{
    ElementSet res;

    if (itsDeleting)
        return res;

    for (auto ve : itsViewElements)
    {
        D1_ASSERT(ve);
        if (ve->IsSelected())
            res.Insert(ve->Element());
    }
    return res;
}


void C::SetSelectionVisibility(bool visible)
{
    if (itsDeleting)
        return;

    if (itsSelectionVisible == visible)
        return;

    itsSelectionVisible = visible;

    for (auto ve : itsViewElements)
    {
        D1_ASSERT(ve);
        auto* sve = ve->Selectable();
        if (sve && sve->IsSelected())
            sve->SelectionNeedsUpdate();
    }
}


void C::NotifySelectionObservers() const
{
    if (itsDeleting)
        return;
    itsSelectionObserverDock.Notify();
}



void C::SetSelection(Selection::Tracker& sc, ElementSet theSelection)
{
    if (itsDeleting)
        return;

    for (auto ve : itsViewElements)
    {
        D1_ASSERT(ve);
        bool selected = theSelection.Contains(ve->Element());
        auto* sve = ve->Selectable();
        if (sve)
            sve->SetSelectionState(sc, selected);
    }
}


auto C::HideSelection() -> Selection::Hider
{
    return itsSelectionVisibilityServer.HideSelection();
}


bool C::HasViewElement(IViewElement& v) const
{
    return itsViewElements.find(&v) != itsViewElements.end();
}


void C::Add(IViewElement& ve)
{
    if (itsDeleting)
        return;

    itsViewElements.insert(&ve);
    // inserts only if ve is not yet contained.

#ifdef _DEBUG
    // some consistency checks
    {
        IElement& me = ve.Element();
        D1_ASSERT(&me);

        IView& view_of_ve = ve.GetView();
        D1_ASSERT(&view_of_ve == this);

        IViewElement* ve_of_me = me.ViewElement(this);
        D1_ASSERT(ve_of_me);
        D1_ASSERT(ve_of_me == &ve);
    }
#endif
}


void C::Add(VISelectable& ve)
{
    if (itsDeleting)
        return;

    itsViewElements.insert(&ve);
    // inserts only if ve is not yet contained.

#ifdef _DEBUG
    // some consistency checks
    {
        IElement& me = ve.Element();
        D1_ASSERT(&me);

        IView& view_of_ve = ve.GetView();
        D1_ASSERT(&view_of_ve == this);

        IViewElement* ve_of_me = me.ViewElement(this);
        D1_ASSERT(ve_of_me);
        D1_ASSERT(ve_of_me == &ve);
    }
#endif
}


void C::Remove(IViewElement& ve)
{
    if (itsDeleting)
        return;

    itsViewElements.erase(&ve);
}


void C::Remove(VISelectable& ve)
{
    if (itsDeleting)
        return;

    itsViewElements.erase(&ve);
}

}
