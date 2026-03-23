/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Transaction;

import :Base;

import d1.Rect;
import d1.Shared;

import std;


namespace Core
{

export class IFollowUpJob
{
public:
    virtual void doFollowUpJob(Env& e) = 0;

protected:
    ~IFollowUpJob() = default;
};


export class FollowUpJob // has value semantics
{
    using SharedPtr = std::shared_ptr<d1::Shared>;

    IFollowUpJob* job_ = nullptr;
    SharedPtr shared_; // controls lifetime

public:
    FollowUpJob() {}

    FollowUpJob(IFollowUpJob* j, const SharedPtr& r):
        job_{ j }, shared_{ r }
    {
    }

    template <class T>
    FollowUpJob(T* t):
        job_{ t }, shared_{ t->shared_from_this() }
    {
    }

    bool operator==(const FollowUpJob& rhs) const
    {
        return rhs.job_ == job_;
    }

    void Do(Env& e)
    {
        if (job_)
            job_->doFollowUpJob(e);
    }
};


export class Transaction
{
    using FollowUps = std::deque<FollowUpJob>;

	class Imp;

    IDiagram& diagram_;
    IView* workingView_; // may be 0
    std::unique_ptr<Imp> imp_;
    FollowUps followUps_;

public:
    Transaction(IDiagram& d, IView* working_view);
    // working view may be 0

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction& rhs) = delete;

    ~Transaction(); // intentionally NOT virtual

    auto diagram() const -> IDiagram& { return diagram_; }
    auto workingView() const -> IView* { return workingView_; }

    auto close(Selection::Tracker&, const IGrid&) -> UndoerRef;
    // Closes this transaction and returns a ref to an undoer that
    // is capable of undoing/redoing this transaction. This includes
    // changes made to clients that were added with addTouched(),
    // trashed clients that were trashed using putIntoTrash() and
    // newly created clients added with addNewlyCreated().
    // Close updates the views of all touched clients.

    void abort();
    // Terminates this transaction and reverts all modifications
    // made so far.

    void addTouched(IElement& me, bool update_view);
    // me may already have been added. If it is readded, its view
    // will be updated on the next UpdateViews call or when the
    // transaction is closed. If newly created model elements are addTouched,
    // they will not be inserted into the touched set, but they are
    // inserted into the needs update view set.
    // PRE: me is not in trash.

    void putIntoTrash(Selection::Tracker&, const IElementRef& me);
    // Trashes me and takes ownership over me. me may have been
    // addTouched() and addNewlyCreated() to this same transaction.

    void addNewlyCreated(IElementRef me);
    // me was newly created during this transaction. The Transaction
    // ensures, that me will be trashed when someone calls undo()
    // on the undoer that is returned by close().

    bool hasNewlyCreated(const IElement& me) const;
    // returns true, if this Transaction has me registered as a newly
    // created client.

    void scheduleFollowUpJob(FollowUpJob);

private:
    auto subTransactionClose(Selection::Tracker&, const IGrid&) -> UndoerRef;

    void makeNew();
};


class Transaction::Imp
{
    friend class Transaction;

    using MESet = std::vector<IElementRef>;
    using FinalizeDeque = std::deque<IElementRef>;

    Transaction& transaction_;
    MESet touchedElements_;
    ElementSet trashedElements_;
    MESet toBeDeletedElements_;
    MESet newlyCreatedElements_;
    IDiagram& diagram_;
    IView* workingView_ = nullptr; // ref only, may be zero
    d1::uint32 number_ = 0;
    bool closed_ = false;
    FinalizeDeque finalizeCandidates_;


public:
    Imp(IDiagram&, IView* working_view, Transaction&);
    // working_view may be zero

    Imp(const Imp&) = delete;
    Imp& operator=(const Imp& rhs) = delete;

    ~Imp();

private:
    bool finalize(Selection::Tracker&, const IGrid&);
    auto close(Selection::Tracker&, const IGrid&) -> UndoerRef;
    void abort();
    void addTouched(IElement& me, bool update_view);
    void putIntoTrash(Selection::Tracker&, const IElementRef& me);
    void addNewlyCreated(IElementRef me);
    bool hasNewlyCreated(const IElement& me) const;

private:
    auto nonNullClose(Selection::Tracker& sc, const IGrid& g) -> UndoerRef;

    auto closeAllTouched() -> UndoerRef;

    void disconnectAllNewlyCreated();
    void disconnectAllTrashed(Selection::Tracker& sc);
    void disconnectAllTouched();

    void abortAllTrashed();
    void abortAllNewlyCreated(Selection::Tracker& sc);

    void destruct();
};


export template <class Data>
class TransactionDataPtr
{
    std::unique_ptr<Data> data_;

public:
    TransactionDataPtr()
    {
    }

    TransactionDataPtr(const TransactionDataPtr&):
        data_{} // Data is not copied!
    {
    }

    TransactionDataPtr& operator=(const TransactionDataPtr& rhs)
    {
        reset();  // rhs is not assigned!
    }

    void reset()
    {
        data_.reset();
    }

    void assign(std::unique_ptr<Data> d)
    {
        data_ = std::move(d);
    }

    Data* get() const { return data_.get(); }
    Data& operator*() const { return *data_; }
    operator bool() const { return data_.get() != nullptr; }

    Data* operator->() const { return data_.get(); }
};


export class Finalizer
{
public:
    Finalizer() = default;
    virtual ~Finalizer() = default;

    Finalizer(const Finalizer&) = delete;
    Finalizer& operator=(const Finalizer& rhs) = delete;

    virtual void execute(Env&) = 0;

    class Dock;
    static auto getDock() -> Dock&;
};


class Finalizer::Dock
{
public:
    virtual void add(std::unique_ptr<Finalizer>) = 0;

    virtual void executeAll(Env&) = 0;

protected:
    ~Dock() = default;
};

}
