/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:View;

import :Base;

import d1.ListSet;
import d1.Rect;

import std;


namespace Core
{

export class IViewElement;
export class VIPointable;
export class VISelectable;
export class ISelectionObserver;


export class IFilter
{
public:
    virtual bool Pass(const VIPointable&) const = 0;

    IFilter() = default;

    virtual ~IFilter() = default;

    IFilter(const IFilter&) = delete;
    IFilter& operator=(const IFilter&) = delete;
};

export using FilterRef = std::shared_ptr<IFilter>;


export class ISelectionObserver
{
public:
    class Dock;

    ISelectionObserver() {}

    ISelectionObserver(const ISelectionObserver&) = delete;
    ISelectionObserver& operator=(const ISelectionObserver&) = delete;

    virtual ~ISelectionObserver();

    virtual void SelectionChanged() = 0;
    // The observed selection may have been changed.

    void SetDock(Dock* d) { itsDock = d; }

private:
    Dock* itsDock = nullptr;
};


class ISelectionObserver::Dock
{
    using SelObsContainer = std::vector<ISelectionObserver*>;
    SelObsContainer itsObservers;

public:
    Dock() {}
    Dock(const Dock&) = delete;
    Dock& operator=(const Dock&) = delete;

    void Attach(ISelectionObserver&);
    void Detach(ISelectionObserver&);
    void Notify() const;
};


export class IView
{
    using ItsViewElementsType = d1::ListSet<IViewElement*>;
    using ItsSelectableViewElementsType = d1::ListSet<VISelectable*>;

    IClub& itsClub;
    ItsViewElementsType itsViewElements; // reference only
    ISelectionObserver::Dock itsSelectionObserverDock;
    Selection::VisibilityServer itsSelectionVisibilityServer{ *this };
    bool itsSelectionVisible = true;
    bool itIsActive = false;
    bool itsDeleting = false;

public:
    IView(IClub&);

    virtual ~IView() = 0; // this DTOR has an implementation

    auto Club() const -> IClub& { return itsClub; }

    virtual void Update() = 0;
    // Updates all outdated IViewElements.

    virtual void ChangeActiveState(bool isActive);
    // if derived class overrides ChangeActiveState, it must call this
    // member explicitly.

    bool IsActive() const { return itIsActive; }

    void Insert(IElement& me, bool update_view = true); // no ownership taken
    // Checks if me already has a view element in this view. If not, it
    // creates a new view element in this view and adds it to the view
    // elements of me.

    virtual void AddToSelection(Selection::Tracker&, const d1::nRect& bounding_box) = 0;
    // Sets the selection based on a bounding box. bounding_box must be
    // normalized.

    auto FindPointable(const d1::Point& p, d1::int32 distance,
        const IFilter* theFilter = 0, bool attaching = false) const -> VIPointable*;
    // Get the VIPointable that says CheckHit() true when the mouse points
    // at position p within distance (p is in logical coordinates).
    // The optional parameter theFilter (reference only) may point
    // to a filter object. If such a filter is supplied, FindPointable()
    // considers only those VIPointable e that have filter.Pass(e) == true

    auto begin() const { return itsViewElements.begin(); }
    auto end() const { return itsViewElements.end(); }

    bool HasSelection() const;
    // Returns true if at least one ViewElement is selected.

    void DeselectAll(Selection::Tracker&);
    void SelectAll(Selection::Tracker&);

    auto Selection() const -> ElementSet;

    void SetSelection(Selection::Tracker&, ElementSet theSelection);
    // POST: all elements which are in "theSelection" are selected.
    //       All others are deselected.

    auto HideSelection() -> Selection::Hider;
    // Hides the selection. The selection is unhidden, after the last
    // Selection::Hider has been destructed. SelectionHiders are allowed
    // to live longer than their view.

    bool SelectionIsVisible() const { return itsSelectionVisible; }

    void Attach(ISelectionObserver& so)
    {
        itsSelectionObserverDock.Attach(so);
    }

    void Detach(ISelectionObserver& so)
    {
        itsSelectionObserverDock.Detach(so);
    }

    void NotifySelectionObservers() const;

    bool HasViewElement(IViewElement& v) const;

    virtual d1::int32 ZoomedDistance(d1::int32 distance) const = 0;
    // Adjusts distance to account for actual zoom factor.

    void SetSelectionVisibility(bool visible);
    // Sets the visibility state of all selected elements.
    // To be called by SelectionVisibilityServerImp.

    void Add(IViewElement& v);
    // Inserts v into its list of view elements if not yet contained.
    // This function is used by IViewElement::Insert.

    void Add(VISelectable& v);
    // Inserts v into its list of selectable view elements if not
    // yet contained. This function is used by
    // ISelectableViewElement::Insert.

    void Remove(IViewElement& v);

    void Remove(VISelectable& v);
};


export class VIPointable // mouse pointable object in a view
{
public:
    virtual auto GetView() const -> IView& = 0;

    virtual bool CheckHit(const d1::Point& pos, d1::int32 distance,
        bool attaching) const = 0;
    // returns true, if this VIPointable feels it is hit, given that
    // the mouse points to position pos within distance.

    virtual auto GetWeight(const d1::Point& pos, d1::int32 distance) const -> Weight = 0;
    // Used in IView::FindPointable(). Usually, the model is consulted to compute
    // the Weight [see IElement::GetWeight()].

    virtual void SetCursor(const d1::Point& mouse_pos) const = 0;

protected:
    ~VIPointable() = default;
};


export class IViewElement: public virtual VIPointable
{
    IView& itsView;

public:
    //-- VIPointable

    bool CheckHit(const d1::Point&, d1::int32, bool attaching) const override;
    auto GetWeight(const d1::Point& pos, d1::int32 distance) const
        -> Weight override;

    //--

    IViewElement(IView& v):
        itsView{ v }
    {
    }

    virtual ~IViewElement();
    // Removes itself from itsIView.

    auto View() const -> IView& { return itsView; }

    virtual void NeedsUpdate() = 0;
    // Inserts itself to the list of outdated elements of its view.

    virtual void CancelUpdate() = 0;
    // Cancels a pending update triggered with NeedsUpdate.

    virtual auto Element() const -> IElement& = 0;

    virtual void ViewDeleted();
    // Someone deleted the View in which this IViewElement is displayed.
    // ViewDeleted() asks the owning model Element of this IViewElement
    // to delete it (the View is *not* the owner of the IViewElements).

    virtual void Insert(bool needsUpdate = true);
    // Inserts itself into its IView and calls NeedsUpdate if needsUpdate
    // is true. It's allowed to call Insert more than once.

    bool IsInActiveView() const;
    // Returns true, if the view of this view element is currently active.

    bool IsSelected() const;

    virtual auto Selectable() -> VISelectable* { return nullptr; }
    virtual auto Selectable() const -> const VISelectable* { return nullptr; }
    // If this view Element supports the Selectable interface, returns
    // a reference pointer to it. Returns zero, if not.

    virtual void TransactionDone() {}
};


export template <class T>
requires std::derived_from<T, IElement>
class Filter: public IFilter
{
    bool Pass(const VIPointable& ps) const override
    {
        using CVE = const IViewElement;
        auto ve = dynamic_cast<CVE*>(&ps);
        if (ve)
        {
            auto& me = ve->Element();
            if (dynamic_cast<T*>(&me))
                return true;
        }
        return false;
    }
};


export class VISelectable: public virtual IViewElement
{
    bool itIsSelected = false;

public:
    //-- IViewElement

    void Insert(bool needsUpdate) override;
    auto Selectable() -> VISelectable* override { return this; }
    auto Selectable() const -> const VISelectable* override { return this; }

    //--

    virtual void SetSelectionState(Selection::Tracker&, bool);
    // selects or deselects a view element.
    // The new selection states becomes immediately visible (no update call
    // needed). The selection state acts only on one view element in a
    // distinct view.

    void ToggleSelectionState(Selection::Tracker& c) { SetSelectionState(c, !itIsSelected); }
    void Select(Selection::Tracker& c) { SetSelectionState(c, true); }
    void Deselect(Selection::Tracker& c) { SetSelectionState(c, false); }

    bool IsSelected() const { return itIsSelected; }

    bool SelectionIsVisible() const;

    virtual void SelectionNeedsUpdate() = 0;
    // Inserts itself in the list of outdated selectables of its view.

    virtual auto FindControl(const d1::Point& p, d1::int32 distance) -> VIPointable*;
    // If this IViewElement has controls, it must find out, if it has a control
    // within distance of p and must return a reference to this Control.
    // If this ViewElement does not have any controls at all or if no Control is
    // within distance of p, it shall return 0.
    // This member function has a default implementation, which always
    // returns 0. Concrete ViewElements which do have controls should override this
    // default implementation.
};

}
