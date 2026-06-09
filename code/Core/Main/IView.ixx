/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:IView;

import Core.SelectionObserver;

import d1.Rect;
import d1.ListSet;


namespace Core
{

export
{
class IClub;
class IElement;
class IFilter;
class VIPointable;
class IViewElement;
class VISelectable;
class ElementSet;

namespace Selection {
class Tracker;
class Hider;
}
}


export class IView
{
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

private:
    using ItsViewElementsType = d1::ListSet<IViewElement*>;
    using ItsSelectableViewElementsType = d1::ListSet<VISelectable*>;

    IClub& club_;
    ItsViewElementsType viewElements_; // reference only
    ISelectionObserver::Dock selectionObserverDock_;
    bool selectionVisible_ = true;
    bool active_ = false;
    bool deleting_ = false;

    struct SelectionDetails;
    std::unique_ptr<SelectionDetails> selection_;
};

}
