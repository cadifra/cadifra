/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:IDiagram;

import :IElement;

import d1.Iterator;
import d1.types;
import d1.Observer;
import d1.Point;

import std;


namespace Core
{

export class IGrid;

export class PosUndoer;


export class Undoer // note: use UndoerRef wherever possible (see below)
{
public:
    virtual ~Undoer() = default;

    virtual bool IsNull() const { return false; }
    // returns true, if this Undoer is a NullUndoer.
    // A NullUndoer does not do anything, if it's Undo or Redo member functions
    // are called. To create a NullUndoer, use UndoerRef::MakeNullUndoer().

	class Param;

    void Undo(Param&);
    void Redo(Param&);

private:
    virtual void UndoImp(Param&) = 0;
    virtual void RedoImp(Param&) = 0;

public:
    virtual bool Merge(Undoer& u) = 0;
    // If possible, Merge retrieves all undo information from u, merges it
    // into this Undoer and returns true.
    // If merging is impossible, Merge makes nothing and returns false.
    // If Merge returns true, the caller may safely delete u without loosing
    // any information.
    //
    // Precondition: This Undoer is older than u
    //
    // Example:
    // You have to Undoers a (older) and b (younger).
    // The following two sequences are identical if Merge returns true:
    //
    //          a.Undo(...)          a.Merge(b)
    //          b.Undo(...)          a.Undo(...)
    //          b.Redo(...)          a.Redo(...)
    //          a.Redo(...)

    virtual PosUndoer* GetPosUndoer() { return 0; }

    virtual void Remove(IElement&) = 0;

protected:
    Undoer()
    {
    }

    Undoer(const Undoer&) = delete;
    Undoer& operator=(const Undoer& rhs) = delete;
};


export class UndoerRef
{
    std::shared_ptr<Undoer> itsUndoer;

public:
    UndoerRef()
    {
        UndoerRef null = MakeNullUndoer();
        itsUndoer = null.itsUndoer;
    }

    UndoerRef(const std::shared_ptr<Undoer>& rhs):
        itsUndoer{ rhs }
    {
    }

    UndoerRef(const UndoerRef& rhs) = default;
    UndoerRef& operator=(const UndoerRef& rhs) = default;

    bool IsNull() const
    {
        return itsUndoer->IsNull();
    }

    static auto MakeNullUndoer() -> UndoerRef;

    void Undo(Undoer::Param& p) const
    {
        itsUndoer->Undo(p);
    }

    void Redo(Undoer::Param& p) const
    {
        itsUndoer->Redo(p);
    }

    auto get() const -> Undoer* { return itsUndoer.get(); }

    bool Merge(UndoerRef u)
    {
        return itsUndoer->Merge(*(u.itsUndoer.get()));
    }
    // If possible, Merge retrieves all undo information from the Undoer referenced
    // by u, merges it into the Undoer referenced by this UndoerRef and returns
    // true.
    // If a merge is impossible, Merge has no effect and returns false.

    void Remove(IElement& me)
    {
        itsUndoer->Remove(me);
    }
};


export class Env
{
public:
    Transaction& transaction;
    Selection::Tracker& sel_tracker;
    const IGrid& grid;

    auto Diagram() const -> IDiagram&;
    auto WorkingView() const -> IView*;

    void AddNewlyCreated(IElementRef me);
    void AddTouched(IElement& me, bool update_view);

    auto Close() -> UndoerRef;
};


class Undoer::Param
{
    using MESet = std::set<IElementRef>;

    MESet itsAddToDiagram;
    MESet itsRemoveFromDiagram;
    MESet itsUpdateViews;

    IDiagram& itsDiagram;
    Selection::Tracker& itsSelectionTracker; // to be removed

public:
    Param(IDiagram& d, Selection::Tracker& sc);

    Param(const Param&) = delete;
    Param& operator=(const Param&) = delete;

    ~Param(); // calls Finish

    void Finish(); // may be called several times

    void AddToDiagram(const IElementRef&);
    void RemoveFromDiagram(const IElementRef&);
    void UpdateViews(const IElementRef&);

    auto Diagram() -> IDiagram& { return itsDiagram; }
};


export class IPosOwner: public virtual IElement
{
public:
    virtual void Move(const d1::Vector& offset) = 0;
};


export class PosUndoer: public Undoer
{
    std::shared_ptr<IPosOwner> itsObject;
    const d1::Vector itsOffset;

public:
    //-- Core::Undoer

    void UndoImp(Param&) override;
    void RedoImp(Param&) override;
    bool Merge(Undoer& u) override;
    PosUndoer* GetPosUndoer() override { return this; }
    void Remove(IElement&) override;
    bool IsNull() const override;

    //--

    PosUndoer(IPosOwner& theObject, const d1::Vector& offset);

    PosUndoer(const PosUndoer&) = delete;
    PosUndoer& operator=(const PosUndoer&) = delete;

    IPosOwner& Object() const { return *itsObject; }
    const d1::Vector& Offset() const { return itsOffset; }
};


export class PosUndoerGroup: public Undoer
{
    using V = std::vector<std::shared_ptr<IPosOwner>>;

    V itsObjects;
    const d1::Vector itsOffset;

public:
    //-- Core::Undoer

    void UndoImp(Param&) override;
    void RedoImp(Param&) override;
    bool Merge(Undoer& u) override;
    void Remove(IElement&) override;
    bool IsNull() const override;

    //--

    PosUndoerGroup(const d1::Vector& offset);

    PosUndoerGroup(const PosUndoerGroup&) = delete;
    PosUndoerGroup& operator=(const PosUndoerGroup&) = delete;

    void Add(IPosOwner&);

    const d1::Vector& Offset() const { return itsOffset; }
};


export class SequenceUndoer: public Undoer
{
    using UndoerListType = std::vector<UndoerRef>;
    UndoerListType ItsUndoerList;

    //-- Undoer

    void UndoImp(Param&) override;
    void RedoImp(Param&) override;
    bool IsNull() const override;
    bool Merge(Undoer& u) override;
    void Remove(IElement&) override;

    //--

public:
    SequenceUndoer() {}
    SequenceUndoer(const SequenceUndoer&) = delete;
    SequenceUndoer& operator=(const SequenceUndoer& rhs) = delete;

public:
    void Append(UndoerRef);
    // Append the Undoer to this SequenceUndoer.
};


export auto Combine(UndoerRef first, UndoerRef second) -> UndoerRef;
//
// Creates an single Undoer from a ordered pair of Undoers.
// In contrast to Under::Merge(), Combine() is always successful.
// first and second can be any Undoer, including NullUndoers.
//
// PRE: first and second must have been created from
// consecutive transactions.


export class TransactionUndoer: public Undoer
{
    using MESet = std::vector<IElementRef>;

    UndoerRef itsTouchedUndoers;
    MESet itsNewlyCreatedElements;
    ElementSet itsTrashedElements;
    MESet itsUntrashedClients;
    ElementSet itsUncreatedClients;
    d1::uint32 itsTransactionNo;

public:
    //-- Undoer

    void UndoImp(Param&) override;
    void RedoImp(Param&) override;
    bool IsNull() const override;
    bool Merge(Undoer& u) override;
    void Remove(IElement&) override;

    //--

    TransactionUndoer(
        UndoerRef theTouchedUndoer,
        const MESet& theNewlyCreatedElements,
        ElementSet theTrashedElements,
        d1::uint32 theTransactionNo);

private:
    static void FindCommonElements(MESet& res, const MESet&, const ElementSet&);
};


export class IDirtyStateObserver
{
public:
    virtual void DirtyStateChanged(bool IsDirty) = 0;

protected:
    ~IDirtyStateObserver() = default;
};


export class IDirtyMarker
{
public:
    IDirtyMarker() = default;
    IDirtyMarker(const IDirtyMarker&) = delete;
    IDirtyMarker& operator=(const IDirtyMarker&) = delete;

    virtual ~IDirtyMarker() = default;

    virtual bool IsDirty() const = 0;

    virtual void ClearDirtyFlag(bool notifyObservers = true) = 0;

    virtual auto GetDirtyStateObserverConnector()
        -> d1::Observer::C<IDirtyStateObserver> = 0;
};


export class IDocumentChangeObserver
{
public:
    virtual void DocumentChanged() = 0;

protected:
    ~IDocumentChangeObserver() = default;
};


export class IUndoRedoCountObserver
{
public:
    virtual void UndoRedoCountChanged(int UndoCount, int RedoCount) = 0;

protected:
    ~IUndoRedoCountObserver() = default;
};


export class IUndoHandler
{
    IDiagram& itsDiagram;

protected:
    IUndoHandler(IDiagram& d):
        itsDiagram{ d }
    {
    }

public:
    virtual ~IUndoHandler() = default;

    IDiagram& GetDiagram() const { return itsDiagram; }


    virtual void AddUndoer(
        UndoerRef u, Selection::IRestorerRef pre, Selection::IRestorerRef post,
        bool merge = false) = 0;
    // Add an Undoer to this UndoHandler. This UndoHandler becomes the owner of
    // the added undoer. The added Undoer is deleted, when this UndoHandler is
    // deleted.
    // 'pre' is responsible to restore the selection state as it was,
    // BEFORE the execution of the user's command that lead to the
    // production of the Undoer u. When this command is undone, the UndoHandler
    // must restore this selection in the active diagram view.
    // 'post' is responsible to restore the selection state as it was,
    // AFTER the execution of the user's command that lead to the
    // production of the Undoer u. When this command is redone, the UndoHandler
    // must restore this selection in the active diagram view.
    // If merge is true, the UndoHandler tries to merge the Undoer u with
    // the Undoer on the top of the undo stack.


    virtual void Undo(IView& activeView, Selection::Tracker&, int numOfUndos) = 0;
    virtual void Redo(IView& activeView, Selection::Tracker&, int numOfRedos) = 0;

    virtual int UndoCount() const = 0;
    virtual int RedoCount() const = 0;


    virtual auto CreateDirtyMarker() -> std::unique_ptr<IDirtyMarker> = 0;

    virtual auto GetDocumentChangeObserverConnector()
        -> d1::Observer::C<IDocumentChangeObserver> = 0;

    virtual auto GetUndoRedoCountObserverConnector()
        -> d1::Observer::C<IUndoRedoCountObserver> = 0;
};


export class IPastePostProcessor
{
public:
    virtual void PastePostProcessing(
        Env&, const ElementSet& pasted_elements) const = 0;

protected:
    ~IPastePostProcessor() = default;
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


export class IDiagram: public IClub
{
    const IPastePostProcessor* const itsPastePostProcessor; // may be zero

public:
    IDiagram(const IPastePostProcessor* p):
        itsPastePostProcessor(p) {}
    IDiagram(const IDiagram&) = delete;
    IDiagram& operator=(const IDiagram&) = delete;

    virtual ~IDiagram() = default;

    auto* GetPastePostProcessor() const { return itsPastePostProcessor; }

    virtual void AddView(IView&) = 0;
    virtual void ForgetView(IView&) = 0;
    virtual auto GetActiveView() const -> IView* = 0;

    virtual auto GetUndoHandler() const -> IUndoHandler& = 0;

    virtual d1::uint32 GetNumOfElements() const = 0;
    // returns the number of elements, which are in this diagram. returns
    // 0, if no diagram is opened.

    virtual auto GetNamedElement(const std::wstring& name) const -> IElement* = 0;

    virtual void Insert(IElementRef) = 0;
    virtual void Remove(IElement&) = 0;

    virtual void AssignIDs() = 0;
    // assigns ObjectIDs to all elements in the diagram that have an ID of zero

    virtual void CreateViewElements(IElement&, bool update_views = true) = 0;

    virtual void DestroyViewElements(IElement&, Selection::Tracker&, IView* active_v = 0) = 0;

    virtual void UpdateViews() = 0;

    virtual bool HasRunningTransaction() const = 0;
    virtual void RegisterTransaction(Transaction&) = 0;
    virtual void UnregisterTransaction(Transaction&) = 0;

    virtual bool HasDoingUndoer() const = 0;
    virtual void BeginUndoRedo(Undoer&) = 0;
    virtual void EndUndoRedo(Undoer&) = 0;

    virtual void Check() const = 0;
};


export class Group
{
public:
    virtual void AssignIDs() = 0;

    virtual auto begin() const -> d1::Iterator<IElement*> = 0;
    virtual auto end() const -> d1::Iterator<IElement*> = 0;

    Group() = default;
    virtual ~Group() = default;

    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    friend auto begin(const Group& g)
    {
        return g.begin();
    }

    friend auto end(const Group& g)
    {
        return g.end();
    }
};


export class TransferSet: public IClub
{
public:
    class Copier;

    using MeSet = std::vector<IElementRef>;

private:
    MeSet itsElements;
    IElement* itsFocus = nullptr; // one of itsElements. ref only
    Copier* itsCopier = nullptr;  // ref only. May be zero

    std::unique_ptr<Group> itsGroup;

public:
    //-- Core::IClub

    auto begin() const -> d1::Iterator<IElement*> override;
    auto end() const -> d1::Iterator<IElement*> override;

    //--

    TransferSet() = default;

    TransferSet(const TransferSet&) = delete;
    TransferSet& operator=(const TransferSet&) = delete;

    virtual ~TransferSet();

    void Add(IElementRef);
    void Remove(IElement&);
    void SetFocus(ObjectID);

    struct PasteRes
    {
        IElement* focus = nullptr; // ref only, may be zero
        ElementSet pasted_elements;
        PasteRes() {}
    };

    auto Paste(Transaction& t) const -> PasteRes;
    // Pastes the contents of the TransferSet into the diagram defined
    // through the active Transaction t.
    // The elements are pasted at the same position from where
    // they were copied.
    // Returns a pointer to the focus element, if there is one in the
    // TransferSet, or zero if there is no focus element.
    // See Inserter::AddFocusCopy below for an explanation of the term
    // "focus element".
    // Note: Paste does NOT change the selection.

    auto StartCopy() -> Copier;
    // Deletes the actual TransferSet content and returns an Copier.
    // The client may then insert copies of model elements into the TransferSet
    // through the Copier.
    // There may only be one active Copier per TransferSet at a time.
    // When the Copier is Completed(), it will execute all necessary
    // actions which are needed to finalize the copy. For example, the
    // Pointers of the inserted model elements must be corrected to
    // point to the elements in the TransferSet. This can only be done until
    // the complete set of the to-be-copied-elements is known.

    bool IsEmpty() const { return itsElements.empty(); }

    auto Focus() const -> IElement* { return itsFocus; }

    auto GetGroup() -> Group&;

    void AssignIDs();

private:
    void Clear();
};


class TransferSet::Copier
{
    friend class TransferSet;

    TransferSet& itsTransferSet;
    std::unique_ptr<CopyRegistry> itsCopyRegistry; // ownership

public:
    Copier(const Copier&) = delete;
    Copier& operator=(const Copier&) = delete;

    auto AddCopy(IElement& m) -> IElement&;
    // Adds a copy of m to the TransferSet which this inserter is bound to.
    // The client must call Complete after the last element has been added.
    // Returns ref to copy

    void AddFocusCopy(IElement& focus);
    // Same as AddCopy, but this element is designated as the focus.
    // The TransferSet will be able to say which element is the focus
    // element on a later paste. The focus element is just one specially
    // marked element - nothing more. Focus elements are used by some tasks.

    void Complete();
    // Must be called after the last element has been added. This
    // completes a copy operation. After Complete has been called, no more
    // elements may be added with AddCopy or AddFocusCopy.
    // The copier may safely be destructed without calling Complete, but
    // this should only happen in an error case as with an exception.
    // If the copier is destructed without a prior call to Complete,
    // the whole copy operation is aborted and the TransferSet is emptied.
    // Complete may be safely called multiple times.

private:
    Copier(TransferSet& c);
};


}
