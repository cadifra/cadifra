/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Element;

import :Selection;

import Core.IGrid;
import Core.ObjectWithID;

import d1.Iterator;
import d1.Rect;
import d1.round;

import std;


namespace Core
{

export {
class CopyRegistry;
class IDiagram;
class IView;
class IViewElement;
class UndoerRef;
class Transaction;
class Weight;
}

export class IElement;


namespace Selection
{
export class Tracker;
}


export using IElementRef = std::shared_ptr<IElement>;


export class ElementSet
{
    std::vector<IElementRef> contents_;

public:
    ElementSet() {}
    ElementSet(const ElementSet& rhs) = default;

    template <class InIter>
    ElementSet(InIter a, InIter b)
    {
        for (; a != b; ++a)
        {
            if (not contains(*a->get()))
                insert(*a->get());
        }
    }

    auto clone() const { return *this; }

    bool insert(IElement&);
    void insert(const ElementSet&);

    bool remove(IElement&);
    bool contains(const IElement&) const;

    auto size() const
    {
        return static_cast<d1::int32>(contents_.size());
    }
    bool empty() const { return contents_.empty(); }
    void clear() { contents_.clear(); }
    auto begin() const { return contents_.begin(); }
    auto end() const { return contents_.end(); }


    void print(std::ostream&) const; // for debugging: writes all IDs

    friend std::ostream& operator<<(std::ostream& s, const ElementSet& d)
    {
        d.print(s);
        return s;
    }

    friend auto begin(const ElementSet& s)
    {
        return s.begin();
    }

    friend auto end(const ElementSet& s)
    {
        return s.end();
    }
};


export class IUndoerCollector
{
public:
    virtual void add(UndoerRef) = 0;

protected:
    ~IUndoerCollector() = default;
};


export struct Env
{
    Transaction& transaction;
    Selection::Tracker& sel_tracker;
    const IGrid& grid;

    auto diagram() const -> IDiagram&;
    auto workingView() const -> IView*;

    void addNewlyCreated(IElementRef me);
    void addTouched(IElement& me, bool update_view);

    auto close() -> UndoerRef;
};


// An IClub contains a set of model elements that may be visible
// in a view. A club is closed, this means that all references
// to elements of all elements in the club refer only to elements
// inside the club.
//
export class IClub
{
public:
    virtual auto begin() const -> d1::Iterator<IElement*> = 0;
    virtual auto end() const -> d1::Iterator<IElement*> = 0;

    friend auto begin(const IClub& c)
    {
        return c.begin();
    }

    friend auto end(const IClub& c)
    {
        return c.end();
    }

protected:
    ~IClub() = default;
};


export class IElement: public ObjectWithID
{
    friend class Transaction;

    struct Rep;
    std::unique_ptr<Rep> rep_;

public:
    class PrivateAccess;

    virtual ~IElement();

    virtual auto copy() const -> IElementRef = 0;
    // Creates a shallow copy of this element.

    virtual void deepInsert(ElementSet& s);
    // The model element must insert elf_ and all its strictly owned dependent
    // model elements into s.

    virtual void adjustRefsAfterCopy(const CopyRegistry& r) = 0;
    // To be called after copy(). Adjusts all references to other elements
    // (of this element) to point to the copies. r contains the mapping
    // from the originals to the copies.
    // AdjustRefsAfterCopy shall be called only once for an element which
    // has been newly created from a Copy call.
    // A set of elements is copied in two phases: first, copy() is called
    // on all elements and second, AdjustRefsAfterCopy is called for
    // each newly created object.
    // If a reference is not contained in the CopyRegistry r. Then the
    // object should set this reference to 0.

    virtual bool acceptForPaste(const IDiagram&) const { return true; }
    // This model element is intended to be pasted into the given diagram. The model
    // element can refuse to be pasted to the diagram by returning false.

    virtual void wasPasted(Env&, ElementSet& pastedElements) {}

    void viewsNeedUpdate(const IDiagram&) const;
    // Calls NeedsUpdate of all its view elements and calls
    // ExtendViewsNeedUpdate of elf_.

    void Delete(IViewElement&);
    // Delete and forget the IViewElement.

    void deleteViewElements(Selection::Tracker&, IView* v);
    // If v is not zero, deletes the view element in v.
    // If v is zero, deletes all view elements of this model element.

    auto makeViewElement(IView& v) -> IViewElement*;
    // creates a new view Element corresponding to the concrete type of this
    // model element. The returned pointer is a reference only.
    // This element is the exclusive owner of the newly created view element and
    // must delete it, when this element is deleted.
    // If this model element already has a view element in v,
    // MakeViewElement does not create a new view element but returns the
    // already existing view element of this model element in v
    // instead.

    virtual auto makeViewElementImp(IView& v) -> std::unique_ptr<IViewElement>;
    // Called by MakeViewElement. To be overridden by derived classes.
    // This member function does have a default implementation, which
    // returns nullptr.

    void setSelectionState(Selection::Tracker&, bool new_state, IView& v);
    // Set the selection state of its view element in v

    bool isSelected(const IView* v) const;
    // return, whether this element is selected in view v. Returns false
    // if v is zero.

    bool isCompletelySelected(const ElementSet& selection) const;
    // For elements like classes: returns true, if class is selected.
    // For segments: returns true if all ends are either at selected
    // elements or unbound.
    // Parameter selection contains all selected elements. Deliberately no
    // view is specified.

    auto viewElement(const IView* v) const -> IViewElement*;
    // Looks through all its view elements and returns the one that is in v.
    // Returns zero if v is zero.

    virtual auto isCopySelected(ExtendSelection::Param&) const -> ExtendSelection::Result;

    void setClub(IClub*);
    auto club() const -> IClub*;

    virtual bool checkHit(
        const d1::Point& pos, d1::int32 distance,
        bool attaching = false) const
    // returns true, if pos is graphically within distance of the shape of
    // this element.
    {
        return false;
    }

    virtual auto getBoundingBox() const -> std::optional<d1::nRect>
    // Visible Elements must return their bounding box.
    // Invisible Elements return std::nullopt
    {
        return std::nullopt;
    }

    virtual auto getWeight(const IView* v, const d1::Point& pos,
        d1::int32 distance) const -> Weight;
    // Returns the weight of this element in view v. v may be legally zero
    // (means there is no view).
    //
    // Weights are used to find which view element is "hit" by a certain mouse
    // position. If more than one element is near a certain mouse position, the
    // Weights of all affected elements are evaluated to discriminate between
    // them. Normally, the element with the largest Weight value will be chosen.
    //
    // This member function is used by the implementation of
    // IView::findPointable().
    //
    // *** WARNING: Failing to correctly implement this function results in
    //              wrong selection behavior (mostly the element of interest
    //              may not be selected by pointing with the mouse cursor).

    //-- storing stuff

    virtual bool storeEnabled() const { return true; }

    virtual auto getNamedObjectName() const -> const wchar_t* { return 0; }
    // Return non-zero if this object is a named object. Named objects are
    // stored under their name.

    struct Register
    {
    public:
        void operator()() {}
    };

    //------

protected:
    IElement();

    IElement(const IElement&);
    // Has the same effect as the default CTOR.

    virtual bool isCompletelySelectedImp(const ElementSet& selection) const;
    // Is called by this->isCompletelySelected().
    //
    // For elements like classes: returns true, if class is selected.
    // For segments: returns true if all ends are either at selected
    // elements or unbound.
    // Parameter selection contains all selected elements. Deliberately no
    // view is specified.
    //
    // Most derived classes override this implementation

public:
    IElement& operator=(const IElement&) = delete;

private:
    virtual void extendViewsNeedUpdate(const IDiagram&) const;
    // Is called by this->viewsNeedUpdate().
    //
    // Derived classes must implement this function to express update dependencies
    // on other model elements. They must call viewsNeedUpdate() on all dependent
    // model elements.
    //
    // If a concrete element does not have update dependencies on other elements
    // it must explicitly provide an empty implementation.
    //
    // *** WARNING: Failing to correctly implement this function results in view
    //              update errors.
    //
    // Example: AssocEnds call viewsNeedUpdate() of their AssocSegment because if
    // the Position of an AssocEnd has changed, its Segment must be redrawn. The
    // Segment would not redraw elf_, because changing the position of the
    // AssocEnd does not touch any data stored in the Segment (Segments only store
    // a references to their segment ends) and therefore the Segment would not be
    // included in the automatic update mechanism.

public:
    auto getTransaction() const -> Transaction*;

    bool isInTransaction() const;
    bool isNewlyCreated() const;
    bool isTouched() const;

    void putIntoTrash(Env& e);
    bool isInTrash() const;

    void setInTrashFlag(bool f);
    // to be called by Transaction

    virtual void finalize(Env& e);
    // A transaction calls Finalize on all touched or newly created model
    // Elements when the transaction is finalized. ModelElements may then execute
    // additional commands. IElement has a dummy implementation of Finalize,
    // which does nothing. Derived classes may override this behaviour.

    virtual void lock() = 0;
    // Attempts to modify locked clients will be caught at runtime. To be called
    // by Transaction. Is called by the transaction for newly created clients when
    // the transaction closes (instead of TransactionClosed call).

    virtual void unlock() = 0;
    // Warning: use this function carefully. Protection is lost if clients are
    // unlocked everywhere. Normally, only clients which are part of a transaction
    // should be in unlocked state.

    virtual bool isLocked() const = 0;


    virtual void setInitialState() = 0;
    // During a transaction, you may optionally call SetInitialState for a newly
    // created IElement. This function marks the actual state of the
    // IElement as the initial state that may be used for any future changes
    // during the running transaction.
    // Precondition:
    // isNewlyCreated() == true and isInTransaction() == true
    //
    // (The BackRep is deleted and recreated. This is important for any functions
    // that use values of the BackRep like Shift etc.)

private:
    virtual void transactionClosed(IUndoerCollector&) = 0;
    // to be called by Transaction

    virtual void transactionAborted() = 0;
    // to be called by Transaction

    void set(Transaction* t);
    // to be called by Transaction

    void setNewlyCreated(bool nc);
    void setTouched(bool t);

    void disconnectTransaction();
    // Resets all Transaction related flags and attributes and locks
    // this client.

protected:
    virtual void transactionDone() {}
    // Derived classes may override this function to get a notification when a
    // transaction is done (closed or aborted).
};


class IElement::PrivateAccess
{
public:
    static void transactionAborted(IElement& me) { me.transactionAborted(); }
    static void setTransaction(IElement& me, Transaction* t) { me.set(t); }
    static void setNewlyCreated(IElement& me, bool nc) { me.setNewlyCreated(nc); }
    static void setTouched(IElement& me, bool t) { me.setTouched(t); }
    static void disconnectTransaction(IElement& me) { me.disconnectTransaction(); }
    static void transactionDone(IElement& me) { me.transactionDone(); }
};


export template <class T>
class Contains
{
    const ElementSet& set_;
    bool found_ = false;

public:
    explicit Contains(const ElementSet& s):
        set_{ s }
    {
    }

    auto get()
    {
        return [&](T* t) {
            if (found_)
                return;
            found_ = set_.contains(*t);
        };
    }

    bool found() const { return found_; }
};

}
