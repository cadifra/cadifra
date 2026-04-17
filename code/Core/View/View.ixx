/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:View;

import :Base;
import :Selection;

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
    virtual bool pass(const VIPointable&) const = 0;

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

    virtual void selectionChanged() = 0;
    // The observed selection may have been changed.

    void setDock(Dock* d) { dock_ = d; }

private:
    Dock* dock_ = nullptr;
};


class ISelectionObserver::Dock
{
    using SelObsContainer = std::vector<ISelectionObserver*>;
    SelObsContainer observers_;

public:
    Dock() {}
    Dock(const Dock&) = delete;
    Dock& operator=(const Dock&) = delete;

    void attach(ISelectionObserver&);
    void detach(ISelectionObserver&);
    void notify() const;
};


export class IView
{
    using ItsViewElementsType = d1::ListSet<IViewElement*>;
    using ItsSelectableViewElementsType = d1::ListSet<VISelectable*>;

    IClub& club_;
    ItsViewElementsType viewElements_; // reference only
    ISelectionObserver::Dock selectionObserverDock_;
    Selection::VisibilityServer selectionVisibilityServer_{ *this };
    bool selectionVisible_ = true;
    bool active_ = false;
    bool deleting_ = false;

public:
    IView(IClub&);

    virtual ~IView() = 0; // this DTOR has an implementation

    auto club() const -> IClub& { return club_; }

    virtual void update() = 0;
    // Updates all outdated IViewElements.

    virtual void changeActiveState(bool isActive);
    // if derived class overrides ChangeActiveState, it must call this
    // member explicitly.

    bool isActive() const { return active_; }

    void insert(IElement& me, bool update_view = true); // no ownership taken
    // Checks if me already has a view element in this view. If not, it
    // creates a new view element in this view and adds it to the view
    // elements of me.

    virtual void addToSelection(Selection::Tracker&, const d1::nRect& bounding_box) = 0;
    // Sets the selection based on a bounding box. bounding_box must be
    // normalized.

    auto findPointable(const d1::Point& p, d1::int32 distance,
        const IFilter* theFilter = 0, bool attaching = false) const -> VIPointable*;
    // Get the VIPointable that says checkHit() true when the mouse points
    // at position p within distance (p is in logical coordinates).
    // The optional parameter theFilter (reference only) may point
    // to a filter object. If such a filter is supplied, findPointable()
    // considers only those VIPointable e that have filter.pass(e) == true

    auto begin() const { return viewElements_.begin(); }
    auto end() const { return viewElements_.end(); }

    bool hasSelection() const;
    // Returns true if at least one ViewElement is selected.

    void deselectAll(Selection::Tracker&);
    void selectAll(Selection::Tracker&);

    auto selection() const -> ElementSet;

    void setSelection(Selection::Tracker&, ElementSet theSelection);
    // POST: all elements which are in "theSelection" are selected.
    //       All others are deselected.

    auto hideSelection() -> Selection::Hider;
    // Hides the selection. The selection is unhidden, after the last
    // Selection::Hider has been destructed. SelectionHiders are allowed
    // to live longer than their view.

    bool selectionIsVisible() const { return selectionVisible_; }

    void attach(ISelectionObserver& so)
    {
        selectionObserverDock_.attach(so);
    }

    void detach(ISelectionObserver& so)
    {
        selectionObserverDock_.detach(so);
    }

    void notifySelectionObservers() const;

    bool has(IViewElement& v) const;

    virtual d1::int32 zoomedDistance(d1::int32 distance) const = 0;
    // Adjusts distance to account for actual zoom factor.

    void setSelectionVisibility(bool visible);
    // Sets the visibility state of all selected elements.
    // To be called by SelectionVisibilityServerImp.

    void add(IViewElement& v);
    // Inserts v into its list of view elements if not yet contained.
    // This function is used by IViewElement::Insert.

    void add(VISelectable& v);
    // Inserts v into its list of selectable view elements if not
    // yet contained. This function is used by
    // ISelectableViewElement::Insert.

    void remove(IViewElement& v);

    void remove(VISelectable& v);
};


export class VIPointable // mouse pointable object in a view
{
public:
    virtual auto getView() const -> IView& = 0;

    virtual bool checkHit(const d1::Point& pos, d1::int32 distance,
        bool attaching) const = 0;
    // returns true, if this VIPointable feels it is hit, given that
    // the mouse points to position pos within distance.

    virtual auto getWeight(const d1::Point& pos, d1::int32 distance) const -> Weight = 0;
    // Used in IView::findPointable(). Usually, the model is consulted to compute
    // the Weight [see IElement::getWeight()].

    virtual void setCursor(const d1::Point& mouse_pos) const = 0;

protected:
    ~VIPointable() = default;
};


export class IViewElement: public virtual VIPointable
{
    IView& view_;

public:
    //-- VIPointable

    bool checkHit(const d1::Point&, d1::int32, bool attaching) const override;
    auto getWeight(const d1::Point& pos, d1::int32 distance) const
        -> Weight override;

    //--

    IViewElement(IView& v):
        view_{ v }
    {
    }

    virtual ~IViewElement();
    // Removes elf_ from iView_.

    auto view() const -> IView& { return view_; }

    virtual void needsUpdate() = 0;
    // Inserts elf_ to the list of outdated elements of its view.

    virtual void cancelUpdate() = 0;
    // Cancels a pending update triggered with NeedsUpdate.

    virtual auto element() const -> IElement& = 0;

    virtual void viewDeleted();
    // Someone deleted the View in which this IViewElement is displayed.
    // viewDeleted() asks the owning model Element of this IViewElement
    // to delete it (the View is *not* the owner of the IViewElements).

    virtual void insert(bool needsUpdate = true);
    // Inserts elf_ into its IView and calls NeedsUpdate if needsUpdate
    // is true. It's allowed to call Insert more than once.

    bool isInActiveView() const;
    // Returns true, if the view of this view element is currently active.

    bool isSelected() const;

    virtual auto selectable() -> VISelectable* { return nullptr; }
    virtual auto selectable() const -> const VISelectable* { return nullptr; }
    // If this view Element supports the Selectable interface, returns
    // a reference pointer to it. Returns zero, if not.

    virtual void transactionDone() {}
};


export template <class T>
requires std::derived_from<T, IElement>
class Filter: public IFilter
{
    bool pass(const VIPointable& ps) const override
    {
        using CVE = const IViewElement;
        auto ve = dynamic_cast<CVE*>(&ps);
        if (ve)
        {
            auto& me = ve->element();
            if (dynamic_cast<T*>(&me))
                return true;
        }
        return false;
    }
};


export class VISelectable: public virtual IViewElement
{
    bool selected_ = false;

public:
    //-- IViewElement

    void insert(bool needsUpdate) override;
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

    bool selectionIsVisible() const;

    virtual void selectionNeedsUpdate() = 0;
    // Inserts elf_ in the list of outdated selectables of its view.

    virtual auto findControl(const d1::Point& p, d1::int32 distance) -> VIPointable*;
    // If this IViewElement has controls, it must find out, if it has a control
    // within distance of p and must return a reference to this Control.
    // If this ViewElement does not have any controls at all or if no Control is
    // within distance of p, it shall return 0.
    // This member function has a default implementation, which always
    // returns 0. Concrete ViewElements which do have controls should override this
    // default implementation.
};

}
