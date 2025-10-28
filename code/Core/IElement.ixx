/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core:IElement;

import d1.ListSet;
import d1.Rect;
import d1.round;
import d1.Shared;

import std;


namespace Core
{

export {
class Env;
class IClub;
class IDiagram;
class IView;
class IViewElement;
class Transaction;
class UndoerRef;
}

export class IElement;
export class IShiftable;


export using IElementRef = std::shared_ptr<IElement>;


export class ElementSet
{
    std::vector<IElementRef> itsContents;

public:
    ElementSet() {}
    ElementSet(const ElementSet& rhs) = default;

    template <class InIter>
    ElementSet(InIter a, InIter b)
    {
        for (; a != b; ++a)
        {
            if (!Contains(*a->get()))
                Insert(*a->get());
        }
    }

    auto Clone() const { return *this; }

    bool Insert(IElement&);
    void Insert(const ElementSet&);

    bool Remove(IElement&);
    bool Contains(const IElement&) const;

    auto size() const
    {
        return static_cast<d1::int32>(itsContents.size());
    }
    bool empty() const { return itsContents.empty(); }
    void clear() { itsContents.clear(); }
    auto begin() const { return itsContents.begin(); }
    auto end() const { return itsContents.end(); }


    void Print(std::ostream&) const; // for debugging: writes all IDs

    friend std::ostream& operator<<(std::ostream& s, const ElementSet& d)
    {
        d.Print(s);
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


namespace ExtendSelection
{

export enum class Result {
    no = 0,
    possibly,
    yes
};


export class Param
{
public:
    class ICaller;

    Param(const ICaller& caller, const ElementSet& selection);
    ~Param();

    Param(const Param&) = delete;
    Param& operator=(const Param&) = delete;

    auto Call(const IElement* target) -> Result;

    auto Selection() const -> const ElementSet& { return itsSelection; }

private:
    class CacheEntry;
    using Cache = std::multimap<const IElement*, CacheEntry>;

    bool FindCacheEntry(const IElement* target, Result& res) const;

    void EraseOtherCacheEntries(const Cache::iterator except);

    Cache itsCache;
    const ICaller& itsCaller;
    const ElementSet& itsSelection;
    const IElement* itsPrevious = nullptr;
};


class Param::ICaller
{
public:
    virtual Result MakeCall(const IElement* target, Param& p) const = 0;

protected:
    ~ICaller() = default;
};

}

namespace Selection
{

export class Tracker
{
    IView* itsView = nullptr; // may be zero (dummy Tracker)
    bool itsSelectionChanged = false;

public:
    Tracker(IView* v):
        itsView{ v }
    {
    }

    Tracker(const Tracker&) = delete;
    Tracker& operator=(const Tracker&) = delete;

    ~Tracker(); // intentionally NOT virtual

    void Changed() { itsSelectionChanged = true; }
};


export class IRestorer
{
public:
    IRestorer() = default;

    virtual ~IRestorer() = default;

    IRestorer(const IRestorer&) = delete;
    IRestorer& operator=(const IRestorer&) = delete;

    virtual void Restore(Tracker&, IView&) = 0;
};

export using IRestorerRef = std::shared_ptr<IRestorer>;


export class Hider;

export class VisibilityServer
{
public:
    VisibilityServer(IView& v):
        itsView{ v }
    {
    }

    VisibilityServer(const VisibilityServer& rhs) = delete;
    VisibilityServer& operator=(const VisibilityServer& rhs) = delete;

    ~VisibilityServer();

    Hider HideSelection();
    // Makes the selection invisible in itsView. If the returned Hider
    // is destructed, the selection is made visible again (if there are no
    // other selection hiders at this VisibilityServer)

    class Imp;

private:
    std::shared_ptr<Imp> itsImp;
    IView& itsView;
};

class VisibilityServer::Imp final
{
    friend class VisibilityServer;
    friend class Hider;

    unsigned long itsNumOfServers{};
    IView* itsView{};
    unsigned long itsNumOfHiders{};

    void AddServer(IView& v);
    void ReleaseServer();

    void AddHider();
    void RemoveHider();
};


export class Hider // has value semantics
{
    using VS = VisibilityServer;

    friend class VS;

    std::shared_ptr<VS::Imp> itsServerImp; // may be zero

public:
    Hider() = default;
    ~Hider(); // intentionally NOT virtual

    Hider(const Hider& rhs);
    Hider& operator=(const Hider& rhs);

private:
    Hider(const std::shared_ptr<VS::Imp>& server);
};

}


export class ObjectID
{
public:
    using T = d1::uint32;

private:
    T v_ = 0;

public:
    ObjectID() {}

    explicit ObjectID(T t):
        v_{ t }
    {
    }

    T val() const
    {
        return v_;
    }

    auto& operator++()
    {
        ++v_;
        return *this;
    }

    friend bool operator==(const ObjectID& a, const ObjectID& b)
    {
        return a.val() == b.val();
    }

    friend bool operator<(const ObjectID& a, const ObjectID& b)
    {
        return a.val() < b.val();
    }

    friend bool operator>(const ObjectID& a, const ObjectID& b)
    {
        return a.val() > b.val();
    }
};


export class ObjectWithID: public d1::Shared
{
    ObjectID itsID{ 0 };

public:
    ObjectWithID() {};

    ObjectID GetID() const { return itsID; }
    void SetID(ObjectID id) { itsID = id; }

    ObjectWithID(const ObjectWithID&):
        itsID{ 0 }
    {
    }

    ObjectWithID& operator=(const ObjectWithID&) = delete;
};


export class Weight
//
//  A weight which is attached to model and view elements.
//  An element with a higher weight is preferred in function
//  IView::FindPointable.
//  "Higher" means in terms of its operator>().
//
{
    class Impl;
    std::shared_ptr<Impl> itsImpl;
    Weight(const std::shared_ptr<Impl>&);

public:
    Weight();
    ~Weight();

    Weight(const Weight&) = default;
    Weight& operator=(const Weight&) = default;

    static auto Invisible() -> Weight;
    static auto Text() -> Weight;

    static auto Control(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& point) -> Weight;

    static auto Point(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& point) -> Weight;

    static auto Line(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& a, const d1::fPoint& b) -> Weight;

    static auto Area(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::nRect& r) -> Weight;

    void IncreaseSelectionBias(const IView* v, const IElement& m);
    // v may be zero (means there is no view).

    void operator+=(const Weight& rhs);

    bool operator==(const Weight& rhs) const;
    bool operator<(const Weight& rhs) const;

    bool operator>(const Weight& rhs) const
    {
        if (this->operator==(rhs))
            return false;
        return !this->operator<(rhs);
    }

    void Print(std::ostream& s) const;

    friend auto operator<<(std::ostream& s, const Weight& w) -> std::ostream&
    {
        w.Print(s);
        return s;
    }
};


export class CopyRegistry
//
// Maintains a mapping from original objects to their copies.
//
{
public:
    static auto MakeNew() -> std::unique_ptr<CopyRegistry>;

    virtual ~CopyRegistry() = default;

    virtual void Register(const IElement* original, IElement* copy) = 0;
    // Stores the mapping from original to copy (ref only pointers).
    // PRE: (1) original != 0 && copy != 0
    //      (2) original and copy have not been registered yet (neither as a copy
    //          nor as an original object)

    virtual IElement* FindCopy(const IElement* original) const = 0;
    // Searches original in the Registry and returns the pointer to its copy if
    // found, zero if not found.
};


export class IUndoerCollector
{
public:
    virtual void Add(UndoerRef) = 0;

protected:
    ~IUndoerCollector() = default;
};


export class IElement: public ObjectWithID
{
    friend class Transaction;

    class Rep;
    std::unique_ptr<Rep> itsRep;

public:
    class PrivateAccess;

    // Make famous type Env available for derived classes in other
    // packages without namespace qualification (only for convenience).
    using Env = Env;

    virtual ~IElement();

    virtual auto Copy() const -> IElementRef = 0;
    // Creates a shallow copy of this element.

    virtual void DeepInsert(ElementSet& s);
    // The model element must insert itself and all its strictly owned dependent
    // model elements into s.

    virtual void AdjustRefsAfterCopy(const CopyRegistry& r) = 0;
    // To be called after Copy(). Adjusts all references to other elements
    // (of this element) to point to the copies. r contains the mapping
    // from the originals to the copies.
    // AdjustRefsAfterCopy shall be called only once for an element which
    // has been newly created from a Copy call.
    // A set of elements is copied in two phases: first, Copy() is called
    // on all elements and second, AdjustRefsAfterCopy is called for
    // each newly created object.
    // If a reference is not contained in the CopyRegistry r. Then the
    // object should set this reference to 0.

    virtual bool AcceptForPaste(const IDiagram&) const { return true; }
    // This model element is intended to be pasted into the given diagram. The model
    // element can refuse to be pasted to the diagram by returning false.

    virtual void WasPasted(Env&, ElementSet& pastedElements) {}

    void ViewsNeedUpdate(const IDiagram&) const;
    // Calls NeedsUpdate of all its view elements and calls
    // ExtendViewsNeedUpdate of itself.

    void DeleteViewElement(IViewElement&);
    // Delete and forget the IViewElement.

    void DeleteViewElements(Selection::Tracker&, IView* v);
    // If v is not zero, deletes the view element in v.
    // If v is zero, deletes all view elements of this model element.

    auto MakeViewElement(IView& v) -> IViewElement*;
    // creates a new view Element corresponding to the concrete type of this
    // model element. The returned pointer is a reference only.
    // This element is the exclusive owner of the newly created view element and
    // must delete it, when this element is deleted.
    // If this model element already has a view element in v,
    // MakeViewElement does not create a new view element but returns the
    // already existing view element of this model element in v
    // instead.

    virtual auto MakeViewElementImp(IView& v) -> std::unique_ptr<IViewElement>;
    // Called by MakeViewElement. To be overridden by derived classes.
    // This member function does have a default implementation, which
    // returns nullptr.

    void SetSelectionState(Selection::Tracker&, bool new_state, IView& v);
    // Set the selection state of its view element in v

    bool IsSelected(const IView* v) const;
    // return, whether this element is selected in view v. Returns false
    // if v is zero.

    bool IsCompletelySelected(const ElementSet& selection) const;
    // For elements like classes: returns true, if class is selected.
    // For segments: returns true if all ends are either at selected
    // elements or unbound.
    // Parameter selection contains all selected elements. Deliberately no
    // view is specified.

    auto ViewElement(const IView* v) const -> IViewElement*;
    // Looks through all its view elements and returns the one that is in v.
    // Returns zero if v is zero.

    virtual auto IsCopySelected(ExtendSelection::Param&) const -> ExtendSelection::Result;

    void SetClub(IClub*);
    auto Club() const -> IClub*;

    virtual bool CheckHit(
        const d1::Point& pos, d1::int32 distance,
        bool attaching = false) const
    // returns true, if pos is graphically within distance of the shape of
    // this element.
    {
        return false;
    }

    virtual auto GetBoundingBox() const -> std::optional<d1::nRect>
    // Visible Elements must return their bounding box.
    // Invisible Elements return std::nullopt
    {
        return std::nullopt;
    }

    virtual auto GetWeight(const IView* v, const d1::Point& pos,
        d1::int32 distance) const -> Weight
    // Returns the weight of this element in view v. v may be legally zero
    // (means there is no view).
    //
    // Weights are used to find which view element is "hit" by a certain mouse
    // position. If more than one element is near a certain mouse position, the
    // Weights of all affected elements are evaluated to discriminate between
    // them. Normally, the element with the largest Weight value will be chosen.
    //
    // This member function is used by the implementation of
    // IView::FindPointable().
    //
    // *** WARNING: Failing to correctly implement this function results in
    //              wrong selection behavior (mostly the element of interest
    //              may not be selected by pointing with the mouse cursor).
    {
        return Core::Weight::Invisible();
    }


    //-- storing stuff

    virtual bool StoreEnabled() const { return true; }

    virtual auto GetNamedObjectName() const -> const wchar_t* { return 0; }
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

    virtual bool IsCompletelySelectedImp(const ElementSet& selection) const;
    // Is called by this->IsCompletelySelected().
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
    virtual void ExtendViewsNeedUpdate(const IDiagram&) const;
    // Is called by this->ViewsNeedUpdate().
    //
    // Derived classes must implement this function to express update dependencies
    // on other model elements. They must call ViewsNeedUpdate() on all dependent
    // model elements.
    //
    // If a concrete element does not have update dependencies on other elements
    // it must explicitly provide an empty implementation.
    //
    // *** WARNING: Failing to correctly implement this function results in view
    //              update errors.
    //
    // Example: AssocEnds call ViewsNeedUpdate() of their AssocSegment because if
    // the Position of an AssocEnd has changed, its Segment must be redrawn. The
    // Segment would not redraw itself, because changing the position of the
    // AssocEnd does not touch any data stored in the Segment (Segments only store
    // a references to their segment ends) and therefore the Segment would not be
    // included in the automatic update mechanism.

public:
    auto GetTransaction() const -> Transaction*;

    bool IsInTransaction() const;
    bool IsNewlyCreated() const;
    bool IsTouched() const;

    void PutIntoTrash(Env& e);
    bool IsInTrash() const;

    void SetInTrashFlag(bool f);
    // to be called by Transaction

    virtual void Finalize(Env& e);
    // A transaction calls Finalize on all touched or newly created model
    // Elements when the transaction is finalized. ModelElements may then execute
    // additional commands. IElement has a dummy implementation of Finalize,
    // which does nothing. Derived classes may override this behaviour.

    virtual void Lock() = 0;
    // Attempts to modify locked clients will be caught at runtime. To be called
    // by Transaction. Is called by the transaction for newly created clients when
    // the transaction closes (instead of TransactionClosed call).

    virtual void Unlock() = 0;
    // Warning: use this function carefully. Protection is lost if clients are
    // unlocked everywhere. Normally, only clients which are part of a transaction
    // should be in unlocked state.

    virtual bool IsLocked() const = 0;


    virtual void SetInitialState() = 0;
    // During a transaction, you may optionally call SetInitialState for a newly
    // created IElement. This function marks the actual state of the
    // IElement as the initial state that may be used for any future changes
    // during the running transaction.
    // Precondition:
    // IsNewlyCreated() == true && IsInTransaction() == true
    //
    // (The BackRep is deleted and recreated. This is important for any functions
    // that use values of the BackRep like Shift etc.)

private:
    virtual void TransactionClosed(IUndoerCollector&) = 0;
    // to be called by Transaction

    virtual void TransactionAborted() = 0;
    // to be called by Transaction

    void SetTransaction(Transaction* t);
    // to be called by Transaction

    void SetNewlyCreated(bool nc);
    void SetTouched(bool t);

    void DisconnectTransaction();
    // Resets all Transaction related flags and attributes and locks
    // this client.

protected:
    virtual void TransactionDone() {}
    // Derived classes may override this function to get a notification when a
    // transaction is done (closed or aborted).
};


class IElement::PrivateAccess
{
public:
    static void TransactionAborted(IElement& me) { me.TransactionAborted(); }
    static void SetTransaction(IElement& me, Transaction* t) { me.SetTransaction(t); }
    static void SetNewlyCreated(IElement& me, bool nc) { me.SetNewlyCreated(nc); }
    static void SetTouched(IElement& me, bool t) { me.SetTouched(t); }
    static void DisconnectTransaction(IElement& me) { me.DisconnectTransaction(); }
    static void TransactionDone(IElement& me) { me.TransactionDone(); }
};


export inline bool IdentCheck(const ObjectWithID* a, const ObjectWithID* b)
{
    return a == b;
}


export inline bool SmallerID(const ObjectWithID* obj1, const ObjectWithID* obj2)
{
    return obj1->GetID() < obj2->GetID();
}


export template <class R, class Pred>
void PrintSortedIDs(std::ostream& os, const R& range, Pred pred)
{
    auto v = std::vector(begin(range), end(range));

    std::ranges::sort(v, pred);

    for (auto* obj : v)
    {
        os << obj->GetID().val();
        os << ",";
    }
}


export template <class R>
void PrintSortedIDs(std::ostream& os, const R& range)
{
    PrintSortedIDs(os, range, SmallerID);
}


export template <typename T, typename C = std::vector<T*>>
class PtrCont: public C
{
public:
    using Eletype = T;

    PtrCont() {}
};


export template <class T>
void AdjustRef(PtrCont<T>& c, const CopyRegistry& r)
{
    auto new_set = PtrCont<T>{};
    new_set.reserve(c.size());

    for (auto* ele : c)
    {
        if (auto* me = r.FindCopy(ele))
        {
            auto new_ele = dynamic_cast<T*>(me);
            D1_ASSERT(new_ele);
            new_set.push_back(new_ele);
        }
    }

    c.assign(begin(new_set), end(new_set));
}



export template <class T>
void AdjustRef(T*& ptr, const CopyRegistry& r)
{
    if (auto* me = r.FindCopy(ptr))
    {
        auto* new_ptr = dynamic_cast<T*>(me);
        D1_ASSERT(new_ptr);
        ptr = new_ptr;
    }
    else
    {
        ptr = 0;
    }
}


export template <class T>
class Contains
{
    const ElementSet& itsSet;
    bool itsFound = false;

public:
    explicit Contains(const ElementSet& s):
        itsSet{ s }
    {
    }

    auto get()
    {
        return [&](T* t) {
            if (itsFound)
                return;
            itsFound = itsSet.Contains(*t);
        };
    }

    bool Found() const { return itsFound; }
};


export class IObjectRegistry
{
public:
    virtual auto Lookup(ObjectID) const -> IElement* = 0;
    // Search for the Object with the given ID.
    // returns 0 if not found.

protected:
    ~IObjectRegistry() = default;
};


export class ObjectRegistry: public IObjectRegistry
{
    using Map = std::map<ObjectID, IElement*>;

    Map itsObjects; // no ownership

public:
    //-- IObjectRegistry

    auto Lookup(ObjectID) const -> IElement* override;

    //--

    void Register(IElement& obj, ObjectID id);

    ObjectRegistry() {}

    ObjectRegistry(const ObjectRegistry&) = delete;
    ObjectRegistry& operator=(const ObjectRegistry&) = delete;
};


export class ShiftVector // has value semantics
{
public:
    d1::fVector distance = {};
    // The distance a shiftable object should shift relative to its old position
    // (old = the position before the transaction started).

    bool shiftX = true; // A shiftable object should not change the x part of its
                        // position if shiftX is false.

    bool shiftY = true; // A shiftable object should not change the y part of its
                        // position if shiftY is false.

    ShiftVector() {}

    ShiftVector(const d1::fVector& distance):
        distance{ distance }
    {
    }

    bool IsNull() const { return !shiftX && !shiftY; }
    // Note: distance == (0,0) means not that a ShiftVector is null!

    d1::fPoint GetNewPos(
        const d1::fPoint& actpos, const d1::fPoint& oldpos) const;

    d1::Point GetNewPos(
        const d1::Point& actpos, const d1::Point& oldpos) const;
};


inline d1::fPoint ShiftVector::GetNewPos(
    const d1::fPoint& actpos, const d1::fPoint& oldpos) const
{
    return {
        shiftX ? oldpos.x + distance.dx : actpos.x,
        shiftY ? oldpos.y + distance.dy : actpos.y
    };
}


inline d1::Point ShiftVector::GetNewPos(
    const d1::Point& actpos, const d1::Point& oldpos) const
{
    return {
        shiftX ? oldpos.x + d1::round(distance.dx) : actpos.x,
        shiftY ? oldpos.y + d1::round(distance.dy) : actpos.y
    };
}


export class DeferredShiftSet
{
    using Shiftables = d1::ListSet<IShiftable*>;
    using Dependents = d1::ListSet<std::pair<
        IShiftable* /*sender*/, IShiftable* /*dependent*/>>;


    ShiftVector itsActualShiftVector;
    d1::fPoint itsActualMousePos;
    IShiftable* itsMasterMover = nullptr; // may be zero
    IShiftable* itsActualSender = nullptr;
    Shiftables itsInstantShiftables, itsDeferredShiftables;
    Dependents itsDependents;

public:
    DeferredShiftSet();
    DeferredShiftSet(const ElementSet& selection, IDiagram&,
        IShiftable* theMasterMover = 0);

    DeferredShiftSet(const DeferredShiftSet&) = delete;
    DeferredShiftSet& operator=(const DeferredShiftSet&) = delete;


    void DeferredShift(Env&, const ShiftVector&, const d1::fPoint& mouse_pos);
    void FinalShift(Env&);

    auto GetDeferredShift() const -> const ShiftVector&;


    void AddDeferredShiftingElement(IShiftable&);

    auto GetMasterMover() const -> IShiftable* { return itsMasterMover; }


    void AddDeferredShiftingElements(const auto& container)
    {
        for (auto* s : container)
            AddDeferredShiftingElement(*s);
    }

    void AddDeferredShiftDependent(IShiftable&);

    void AddDeferredShiftDependents(const auto& container)
    {
        for (auto* s : container)
            AddDeferredShiftDependent(*s);
    }

    template <class Iter>
    void AddDeferredShiftDependents(Iter a, Iter b)
    {
        for (; a != b; ++a)
            AddDeferredShiftDependent(**a);
    }

    bool IsDeferredShifting(const IShiftable&) const;

    auto begin() const { return itsDeferredShiftables.begin(); }
    auto end() const { return itsDeferredShiftables.end(); }

    void Print(std::ostream&) const; // for debugging

    friend std::ostream& operator<<(std::ostream& s, const DeferredShiftSet& d)
    {
        d.Print(s);
        return s;
    }
};


export class IShiftable: public virtual IElement
{
public:
    IShiftable() {}
    //  IShiftable(const IShiftable&)   generated by compiler

    IShiftable& operator=(const IShiftable&) = delete;

    void Shift(Core::Env&,
        IElement* sender,
        const ShiftVector&,
        const DeferredShiftSet&,
        bool shallow);

    virtual void ShiftImpl(Core::Env&,
        IElement* sender,
        const ShiftVector&,
        const DeferredShiftSet&,
        bool shallow) = 0;

    virtual void AddDeferredShiftingElements(
        DeferredShiftSet&, const ElementSet& selection) = 0;

    virtual void AddDeferredShiftDependents(DeferredShiftSet&) = 0;

    virtual void DetachFromSource(Core::Env&,
        const DeferredShiftSet&, const ElementSet& selection);

    virtual void ExtendSelection(
        const ElementSet& selection,
        ElementSet& extendedSelectionSet);
};


export class ActorAddDeferredShiftDependent
{
    DeferredShiftSet& dss;

public:
    ActorAddDeferredShiftDependent(DeferredShiftSet& dss):
        dss{ dss }
    {
    }

    void operator()(IShiftable* t)
    {
        dss.AddDeferredShiftDependent(*t);
    }
};


export namespace CopyExtendSelection
{

auto Build(const ElementSet& selection, IDiagram&) -> ElementSet;

}


export auto MakeStandardSelectionRestorer(
    const ElementSet& selection) -> Selection::IRestorerRef;

}
