/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.TransactionUndoer;

import Core.Main;

import d1.types;

import std;


namespace Core
{

export class TransactionUndoer: public Undoer
{
public:
    //-- Undoer

    void undoImp(Param&) override;
    void redoImp(Param&) override;
    bool isNull() const override;
    bool merge(Undoer& u) override;
    void remove(IElement&) override;

    //--

    using MESet = std::vector<IElementRef>;

    TransactionUndoer(
        UndoerRef theTouchedUndoer,
        const MESet& theNewlyCreatedElements,
        ElementSet theTrashedElements,
        d1::uint32 theTransactionNo);

private:
    static void findCommonElements(MESet& res, const MESet&, const ElementSet&);


    UndoerRef touchedUndoers_;
    MESet newlyCreatedElements_;
    ElementSet trashedElements_;
    MESet untrashedClients_;
    ElementSet uncreatedClients_;
    d1::uint32 transactionNo_;
};

}

module :private;

import d1.algorithm;

import WinUtil.Debug;


namespace Core
{

#ifdef _DEBUG
bool debugOutputEnabled()
{
    return (WinUtil::DebugEnv::inst().getInt("Core", "Transaction::Debug") != 0);
}
#endif


TransactionUndoer::TransactionUndoer(
    UndoerRef theTouchedUndoer,
    const MESet& theNewlyCreatedElements,
    ElementSet theTrashedElements,
    d1::uint32 theTransactionNo):

    touchedUndoers_{ theTouchedUndoer },
    newlyCreatedElements_{ theNewlyCreatedElements },
    trashedElements_{ theTrashedElements },
    transactionNo_{ theTransactionNo }
{
}


void TransactionUndoer::undoImp(Param& p)
{
    for (auto me : trashedElements_)
    {
        D1_ASSERT(me);
        me->setInTrashFlag(false);
        p.addToDiagram(me);
        untrashedClients_.push_back(me);
    }

    trashedElements_.clear();

    touchedUndoers_.undo(p);

    while (not newlyCreatedElements_.empty())
    {
        IElementRef me = newlyCreatedElements_.back();
        D1_ASSERT(me);
        newlyCreatedElements_.pop_back();
        uncreatedClients_.insert(*me.get());
        me->setInTrashFlag(true);
        p.removeFromDiagram(me);
    }


#ifdef _DEBUG
    if (debugOutputEnabled())
        WinUtil::dout
            << "Undone Transaction " << transactionNo_ << std::endl;
#endif
}


void TransactionUndoer::redoImp(Param& p)
{
    for (auto me : uncreatedClients_)
    {
        D1_ASSERT(me);
        newlyCreatedElements_.push_back(me);
        me->setInTrashFlag(false);
        p.addToDiagram(me);
    }

    uncreatedClients_.clear();

    touchedUndoers_.redo(p);

    while (not untrashedClients_.empty())
    {
        auto me = untrashedClients_.back();
        D1_ASSERT(me);
        untrashedClients_.pop_back();
        trashedElements_.insert(*me);
        me->setInTrashFlag(true);
        p.removeFromDiagram(me);
    }

#ifdef _DEBUG
    if (debugOutputEnabled())
        WinUtil::dout
            << "Redone Transaction " << transactionNo_ << std::endl;
#endif
}


bool TransactionUndoer::isNull() const
{
    return touchedUndoers_.isNull() and
           newlyCreatedElements_.empty() and
           trashedElements_.empty() and
           untrashedClients_.empty() and
           uncreatedClients_.empty();
}


bool TransactionUndoer::merge(Undoer& u)
{
    auto* t = dynamic_cast<TransactionUndoer*>(&u);

    if (not t)
        return false;

    if (not touchedUndoers_.merge(t->touchedUndoers_))
        return false;

    for (auto me : t->newlyCreatedElements_)
        d1::push_back_if_missing(newlyCreatedElements_, me);

    t->newlyCreatedElements_.clear();


    trashedElements_.insert(t->trashedElements_);


    for (auto me : t->untrashedClients_)
        d1::push_back_if_missing(untrashedClients_, me);

    t->untrashedClients_.clear();


    uncreatedClients_.insert(t->uncreatedClients_);

    MESet r;
    findCommonElements(r, newlyCreatedElements_, trashedElements_);
    findCommonElements(r, untrashedClients_, uncreatedClients_);

    for (auto me : r)
        remove(*me.get());

    return true;
}


void TransactionUndoer::remove(IElement& me)
{
    touchedUndoers_.remove(me);
    d1::erase_first_with_get(newlyCreatedElements_, me);
    trashedElements_.remove(me);
    d1::erase_first_with_get(untrashedClients_, me);
    uncreatedClients_.remove(me);
}


void TransactionUndoer::findCommonElements(
    MESet& res, const MESet& a, const ElementSet& b)
{
    auto f = [&](auto& m) { return b.contains(*m.get()); };

    for (auto& m : a | std::views::filter(f))
        res.push_back(m);
}

}
