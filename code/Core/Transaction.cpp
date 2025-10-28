/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

import d1.Exception;
import d1.algorithm;


namespace Core
{

namespace
{
using C = Transaction;
}


C::Transaction(IDiagram& d, IView* v):
    itsDiagram{ d },
    itsWorkingView{ v }
{
}


C::~Transaction() = default;


auto C::Close(Selection::Tracker& sc, const IGrid& g) -> UndoerRef
{
    class UndoOnDelete
    {
        Env itsEnv;
        UndoerRef itsUndoer;

    public:
        UndoOnDelete(Transaction& t, Selection::Tracker& sc, const IGrid& g):
            itsEnv{ t, sc, g }
        {
        }

        ~UndoOnDelete()
        {
            auto up = Undoer::Param{ itsEnv.Diagram(), itsEnv.sel_tracker };
            itsUndoer.Undo(up);
        }

        auto Release() -> UndoerRef
        {
            UndoerRef u;
            std::swap(itsUndoer, u);
            return u;
        }

        void Add(UndoerRef u) { itsUndoer = Combine(itsUndoer, u); }

        auto GetEnv() -> Env& { return itsEnv; }

    } u{ *this, sc, g };

    unsigned int maxrepeat = 10000 + itsDiagram.GetNumOfElements();

    for (;;)
    {
        u.Add(SubTransactionClose(sc, g));

        if (itsFollowUps.empty())
            break;

        if (!--maxrepeat)
        {
            itsFollowUps.clear();

            class E: public d1::Exception
            {
                const char* what() const { return "Transaction::Close overflow"; }
            };

            throw E{};
        }

        auto f = FollowUpJob{ itsFollowUps.front() };
        itsFollowUps.pop_front();

        f.Do(u.GetEnv());
    }

    return u.Release();
}


auto C::SubTransactionClose(Selection::Tracker& sc, const IGrid& g) -> UndoerRef
{
    if (!itsImp)
        return {};

    bool finalize_successful = itsImp->Finalize(sc, g);
    auto res = itsImp->Close(sc, g);
    itsImp.reset();

    if (finalize_successful)
        return res;

    auto up = Undoer::Param{ itsDiagram, sc };
    res.Undo(up);

    class E: public d1::Exception
    {
        const char* what() const
        {
            return "TransactionImp::Finalize overflow";
        }
    };

    throw E{};
}


void C::Abort()
{
    if (itsImp)
    {
        itsImp->Abort();
        itsImp.reset();
    }
    itsFollowUps.clear();
}


void C::MakeNew()
{
    if (!itsImp)
        itsImp = std::make_unique<Imp>(itsDiagram, itsWorkingView, *this);
}


void C::AddTouched(IElement& me, bool update_view)
{
    MakeNew();
    D1_ASSERT(itsImp.get());
    itsImp->AddTouched(me, update_view);
}


void C::PutIntoTrash(Selection::Tracker& sc, const IElementRef& me)
{
    MakeNew();
    D1_ASSERT(itsImp.get());
    itsImp->PutIntoTrash(sc, me);
}


void C::AddNewlyCreated(IElementRef me)
{
    MakeNew();
    D1_ASSERT(itsImp.get());
    itsImp->AddNewlyCreated(me);
}


bool C::HasNewlyCreated(const IElement& me) const
{
    if (itsImp)
        return itsImp->HasNewlyCreated(me);

    return false;
}


void C::ScheduleFollowUpJob(FollowUpJob f)
{
    d1::push_back_if_missing(itsFollowUps, f);
}

}
