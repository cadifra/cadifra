/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.Main:IViewElement;

import :IElement;
import :IView;
import :IFilter;
import :ElementSet;
import :VIPointable;
import :SelectionTracker;
import :SelectionVisibilityServer;

import Core.Weight;

import d1.Point;

import WinUtil.Debug;


namespace Core
{

export
{
class IView;
class VISelectable;
}


export class IViewElement: public virtual VIPointable
{
    IView& view_;

public:
    //-- VIPointable

    bool checkHit(const d1::Point& pos, d1::int32 dist, bool attaching) const override
    {
        return element().checkHit(pos, dist, attaching);
    }

    auto getWeight(const d1::Point& pos, d1::int32 distance) const
        -> Weight override
    {
        return element().getWeight(&view_, pos, distance);
    }

    //--

    IViewElement(IView& v):
        view_{ v }
    {
    }

    virtual ~IViewElement()
    {
        view_.remove(*this);
    }

    auto view() const -> IView& { return view_; }

    virtual void needsUpdate() = 0;
    // Inserts elf_ to the list of outdated elements of its view.

    virtual void cancelUpdate() = 0;
    // Cancels a pending update triggered with NeedsUpdate.

    virtual auto element() const -> IElement& = 0;

    virtual void viewDeleted() { element().Delete(*this); }

    virtual void insert(bool needs_update = true)
    // Note: It's allowed to call Insert more than once.
    {
        view_.add(*this);

        D1_ASSERT(&view_.club() == element().club());

        if (needs_update)
            needsUpdate();
    }

    bool isInActiveView() const { return view_.isActive(); }

    bool isSelected() const;

    virtual auto selectable() -> VISelectable* { return nullptr; }
    virtual auto selectable() const -> const VISelectable* { return nullptr; }
    // If this view Element supports the Selectable interface, returns
    // a reference pointer to it. Returns zero, if not.

    virtual void transactionDone() {}
};


export class VISelectable: public virtual IViewElement
{
    bool selected_ = false;

public:
    //-- IViewElement

    void insert(bool needs_update) override
    {
        getView().add(*this);

        if (needs_update)
            needsUpdate();
    }

    auto selectable() -> VISelectable* override { return this; }
    auto selectable() const -> const VISelectable* override { return this; }

    //--

    virtual void setSelectionState(Selection::Tracker&, bool);
    // selects or deselects a view element.
    // The new selection states becomes immediately visible (no update call
    // needed). The selection state acts only on one view element in a
    // distinct view.

    void toggleSelectionState(Selection::Tracker& c) { setSelectionState(c, not selected_); }
    void select(Selection::Tracker& c) { setSelectionState(c, true); }
    void deselect(Selection::Tracker& c) { setSelectionState(c, false); }

    bool isSelected() const { return selected_; }

    bool selectionIsVisible() const
    {
        return getView().selectionIsVisible();
    }

    virtual void selectionNeedsUpdate() = 0;
    // Inserts elf_ in the list of outdated selectables of its view.

    virtual auto findControl(const d1::Point& p, d1::int32 distance) -> VIPointable*
    // If this IViewElement has controls, it must find out, if it has a control
    // within distance of p and must return a reference to this Control.
    // If this ViewElement does not have any controls at all or if no Control is
    // within distance of p, it shall return 0.
    // This member function has a default implementation, which always
    // returns 0. Concrete ViewElements which do have controls should override this
    // default implementation.
    {
        return nullptr;
    }
};


bool IViewElement::isSelected() const
{
    auto* sve = selectable();
    return sve and sve->isSelected();
}


void VISelectable::setSelectionState(Selection::Tracker& st, bool newstate)
{
    bool change = selected_ != newstate;

    if (change)
    {
        selected_ = newstate;
        st.changed();
    }

    auto& me = element();
    D1_ASSERT(&me);

    if (change)
        selectionNeedsUpdate();

#ifdef _DEBUG
    // some consistency checks
    {
        IView& view_of_this = getView();
        IViewElement* ve_of_me = me.viewElement(&view_of_this);
        D1_ASSERT(ve_of_me);
        D1_ASSERT(ve_of_me == this);
    }
#endif
}

}


namespace Core
{

using std::ranges::for_each;


struct IView::SelectionDetails
{
    Selection::VisibilityServer visibilityServer;

    SelectionDetails(IView& v):
        visibilityServer(v)
    {
    }
};


IView::IView(IClub& c):
    club_{ c },
    selection_{ std::make_unique<SelectionDetails>(*this) }
{
}


void IView::changeActiveState(bool active)
{
    if (deleting_)
        return;

    if (active == active_)
        return; // no change

    active_ = active;
}


IView::~IView()
{
    deleting_ = true;

    auto a = [&](auto* ve) { ve->viewDeleted(); };
    for_each(viewElements_, a);
}


void IView::insert(IElement& me, bool update_view)
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
d1::uint32 gDebugFindPointableAmbiguous = 0;
bool findPointableDebugEnabled()
{
    return (WinUtil::DebugEnv::inst().getInt("Core", "IView::DebugFindPointable") != 0);
}
#endif


auto IView::findPointable(const d1::Point& p, d1::int32 unzoomed_distance,
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


bool IView::hasSelection() const
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


void IView::deselectAll(Selection::Tracker& sc)
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


void IView::selectAll(Selection::Tracker& sc)
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


auto IView::selection() const -> ElementSet
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


void IView::setSelectionVisibility(bool visible)
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


void IView::notifySelectionObservers() const
{
    if (deleting_)
        return;
    selectionObserverDock_.notify();
}



void IView::setSelection(Selection::Tracker& sc, ElementSet theSelection)
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


auto IView::hideSelection() -> Selection::Hider
{
    return selection_->visibilityServer.hideSelection();
}


bool IView::has(IViewElement& v) const
{
    return viewElements_.find(&v) != viewElements_.end();
}


void IView::add(IViewElement& ve)
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


void IView::add(VISelectable& ve)
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


void IView::remove(IViewElement& ve)
{
    if (deleting_)
        return;

    viewElements_.erase(&ve);
}


void IView::remove(VISelectable& ve)
{
    if (deleting_)
        return;

    viewElements_.erase(&ve);
}

}
