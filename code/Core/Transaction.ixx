/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Transaction;

import :IElement;

import d1.Rect;
import d1.Shared;

import std;


namespace Core
{

export class IFollowUpJob
{
public:
    virtual void DoFollowUpJob(Env& e) = 0;

protected:
    ~IFollowUpJob() = default;
};


export class FollowUpJob // has value semantics
{
    using SharedPtr = std::shared_ptr<d1::Shared>;

    IFollowUpJob* itsJob = nullptr;
    SharedPtr itsShared; // controls lifetime

public:
    FollowUpJob() {}

    FollowUpJob(IFollowUpJob* j, const SharedPtr& r):
        itsJob{ j }, itsShared{ r }
    {
    }

    template <class T>
    FollowUpJob(T* t):
        itsJob{ t }, itsShared{ t->shared_from_this() }
    {
    }

    bool operator==(const FollowUpJob& rhs) const
    {
        return rhs.itsJob == itsJob;
    }

    void Do(Env& e)
    {
        if (itsJob)
            itsJob->DoFollowUpJob(e);
    }
};


export class IGrid
{
public:
    virtual d1::Point ToGrid(const d1::Point& p) const = 0;
    // find nearest position on grid to.

    virtual d1::int32 EnlargeToGrid(d1::int32 d) const = 0;

    virtual d1::Vector EnlargeToGrid(const d1::Vector& v) const = 0;
    // if the coordinates of v are not in even grid spaces, enlarge them
    // to next multiple.

    virtual d1::Size EnlargeToGrid(const d1::Size& p) const = 0;

    virtual d1::nRect EnlargeToGrid(const d1::nRect&) const = 0;

protected:
    ~IGrid() = default;
};


export class Transaction
{
    using FollowUps = std::deque<FollowUpJob>;

	class Imp;

    IDiagram& itsDiagram;
    IView* itsWorkingView; // may be 0
    std::unique_ptr<Imp> itsImp;
    FollowUps itsFollowUps;

public:
    Transaction(IDiagram& d, IView* working_view);
    // working view may be 0

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction& rhs) = delete;

    ~Transaction(); // intentionally NOT virtual

    auto Diagram() const -> IDiagram& { return itsDiagram; }
    auto WorkingView() const -> IView* { return itsWorkingView; }

    auto Close(Selection::Tracker&, const IGrid&) -> UndoerRef;
    // Closes this transaction and returns a ref to an undoer that
    // is capable of undoing/redoing this transaction. This includes
    // changes made to clients that were added with AddTouched(),
    // trashed clients that were trashed using PutIntoTrash() and
    // newly created clients added with AddNewlyCreated().
    // Close updates the views of all touched clients.

    void Abort();
    // Terminates this transaction and reverts all modifications
    // made so far.

    void AddTouched(IElement& me, bool update_view);
    // me may already have been added. If it is readded, its view
    // will be updated on the next UpdateViews call or when the
    // transaction is closed. If newly created model elements are AddTouched,
    // they will not be inserted into the touched set, but they are
    // inserted into the needs update view set.
    // PRE: me is not in trash.

    void PutIntoTrash(Selection::Tracker&, const IElementRef& me);
    // Trashes me and takes ownership over me. me may have been
    // AddTouched() and AddNewlyCreated() to this same transaction.

    void AddNewlyCreated(IElementRef me);
    // me was newly created during this transaction. The Transaction
    // ensures, that me will be trashed when someone calls Undo()
    // on the undoer that is returned by Close().

    bool HasNewlyCreated(const IElement& me) const;
    // returns true, if this Transaction has me registered as a newly
    // created client.

    void ScheduleFollowUpJob(FollowUpJob);

private:
    auto SubTransactionClose(Selection::Tracker&, const IGrid&) -> UndoerRef;

    void MakeNew();
};


class Transaction::Imp
{
    friend class Transaction;

    using MESet = std::vector<IElementRef>;
    using FinalizeDeque = std::deque<IElementRef>;

    Transaction& itsTransaction;
    MESet itsTouchedElements;
    ElementSet itsTrashedElements;
    MESet itsToBeDeletedElements;
    MESet itsNewlyCreatedElements;
    IDiagram& itsDiagram;
    IView* itsWorkingView = nullptr; // ref only, may be zero
    d1::uint32 itsNumber = 0;
    bool itIsClosed = false;
    FinalizeDeque itsFinalizeCandidates;


public:
    Imp(IDiagram&, IView* working_view, Transaction&);
    // working_view may be zero

    Imp(const Imp&) = delete;
    Imp& operator=(const Imp& rhs) = delete;

    ~Imp();

private:
    bool Finalize(Selection::Tracker&, const IGrid&);
    auto Close(Selection::Tracker&, const IGrid&) -> UndoerRef;
    void Abort();
    void AddTouched(IElement& me, bool update_view);
    void PutIntoTrash(Selection::Tracker&, const IElementRef& me);
    void AddNewlyCreated(IElementRef me);
    bool HasNewlyCreated(const IElement& me) const;

private:
    auto NonNullClose(Selection::Tracker& sc, const IGrid& g) -> UndoerRef;

    auto CloseAllTouched() -> UndoerRef;

    void DisconnectAllNewlyCreated();
    void DisconnectAllTrashed(Selection::Tracker& sc);
    void DisconnectAllTouched();

    void AbortAllTrashed();
    void AbortAllNewlyCreated(Selection::Tracker& sc);

    void Destruct();
};


export template <class Data>
class TransactionDataPtr
{
    std::unique_ptr<Data> itsData;

public:
    TransactionDataPtr()
    {
    }

    TransactionDataPtr(const TransactionDataPtr&):
        itsData{} // Data is not copied!
    {
    }

    TransactionDataPtr& operator=(const TransactionDataPtr& rhs)
    {
        reset();  // rhs is not assigned!
    }

    void reset()
    {
        itsData.reset();
    }

    void assign(std::unique_ptr<Data> d)
    {
        itsData = std::move(d);
    }

    Data* get() const { return itsData.get(); }
    Data& operator*() const { return *itsData; }
    operator bool() const { return itsData.get() != nullptr; }

    Data* operator->() const { return itsData.get(); }
};


export class Finalizer
{
public:
    Finalizer() = default;
    virtual ~Finalizer() = default;

    Finalizer(const Finalizer&) = delete;
    Finalizer& operator=(const Finalizer& rhs) = delete;

    virtual void Execute(Env&) = 0;

    class Dock;
    static auto GetDock() -> Dock&;
};


class Finalizer::Dock
{
public:
    virtual void Add(std::unique_ptr<Finalizer>) = 0;

    virtual void ExecuteAll(Env&) = 0;

protected:
    ~Dock() = default;
};

}
