/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:Transaction;

import :IElement;
import :FollowUpJob;

import Core.Grid;

import std;


namespace Core
{

export
{
class IDiagram;
class IView;
class UndoerRef;
}


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

}
