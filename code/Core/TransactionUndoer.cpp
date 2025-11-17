/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

import :Undoer;

import d1.algorithm;

import WinUtil.Debug;


namespace Core
{


#ifdef _DEBUG
namespace
{
bool DebugOutputEnabled()
{
    return (WinUtil::DebugEnv::Inst().GetInt("Core", "Transaction::Debug") != 0);
}
}
#endif

namespace
{
using C = TransactionUndoer;
}


C::TransactionUndoer(
    UndoerRef theTouchedUndoer,
    const MESet& theNewlyCreatedElements,
    ElementSet theTrashedElements,
    d1::uint32 theTransactionNo):

    itsTouchedUndoers{ theTouchedUndoer },
    itsNewlyCreatedElements{ theNewlyCreatedElements },
    itsTrashedElements{ theTrashedElements },
    itsTransactionNo{ theTransactionNo }
{
}


void C::UndoImp(Param& p)
{
    for (auto me : itsTrashedElements)
    {
        D1_ASSERT(me);
        me->SetInTrashFlag(false);
        p.AddToDiagram(me);
        itsUntrashedClients.push_back(me);
    }

    itsTrashedElements.clear();

    itsTouchedUndoers.Undo(p);

    while (!itsNewlyCreatedElements.empty())
    {
        IElementRef me = itsNewlyCreatedElements.back();
        D1_ASSERT(me);
        itsNewlyCreatedElements.pop_back();
        itsUncreatedClients.Insert(*me.get());
        me->SetInTrashFlag(true);
        p.RemoveFromDiagram(me);
    }


#ifdef _DEBUG
    if (DebugOutputEnabled())
        WinUtil::dout
            << "Undone Transaction " << itsTransactionNo << std::endl;
#endif
}


void C::RedoImp(Param& p)
{
    for (auto me : itsUncreatedClients)
    {
        D1_ASSERT(me);
        itsNewlyCreatedElements.push_back(me);
        me->SetInTrashFlag(false);
        p.AddToDiagram(me);
    }

    itsUncreatedClients.clear();

    itsTouchedUndoers.Redo(p);

    while (!itsUntrashedClients.empty())
    {
        auto me = itsUntrashedClients.back();
        D1_ASSERT(me);
        itsUntrashedClients.pop_back();
        itsTrashedElements.Insert(*me);
        me->SetInTrashFlag(true);
        p.RemoveFromDiagram(me);
    }

#ifdef _DEBUG
    if (DebugOutputEnabled())
        WinUtil::dout
            << "Redone Transaction " << itsTransactionNo << std::endl;
#endif
}


bool C::IsNull() const
{
    return itsTouchedUndoers.IsNull() &&
           itsNewlyCreatedElements.empty() &&
           itsTrashedElements.empty() &&
           itsUntrashedClients.empty() &&
           itsUncreatedClients.empty();
}


bool C::Merge(Undoer& u)
{
    auto* t = dynamic_cast<TransactionUndoer*>(&u);

    if (!t)
        return false;

    if (!itsTouchedUndoers.Merge(t->itsTouchedUndoers))
        return false;

    for (auto me : t->itsNewlyCreatedElements)
        d1::push_back_if_missing(itsNewlyCreatedElements, me);

    t->itsNewlyCreatedElements.clear();


    itsTrashedElements.Insert(t->itsTrashedElements);


    for (auto me : t->itsUntrashedClients)
        d1::push_back_if_missing(itsUntrashedClients, me);

    t->itsUntrashedClients.clear();


    itsUncreatedClients.Insert(t->itsUncreatedClients);

    MESet remove;
    FindCommonElements(remove, itsNewlyCreatedElements, itsTrashedElements);
    FindCommonElements(remove, itsUntrashedClients, itsUncreatedClients);

    for (auto me : remove)
        Remove(*me.get());

    return true;
}


void C::Remove(IElement& me)
{
    itsTouchedUndoers.Remove(me);
    d1::erase_first_with_get(itsNewlyCreatedElements, me);
    itsTrashedElements.Remove(me);
    d1::erase_first_with_get(itsUntrashedClients, me);
    itsUncreatedClients.Remove(me);
}


void C::FindCommonElements(MESet& res, const MESet& a, const ElementSet& b)
{
    for (auto mi : a)
        if (b.Contains(*mi.get()))
            res.push_back(mi);
}

}
