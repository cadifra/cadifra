/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core:Transaction.Transaction;
import :Transaction;

import :Diagram;

import d1.Exception;
import d1.algorithm;


namespace Core
{

namespace
{
using C = Transaction;
}


C::Transaction(IDiagram& d, IView* v):
    diagram_{ d },
    workingView_{ v }
{
}


C::~Transaction() = default;


auto C::close(Selection::Tracker& sc, const IGrid& g) -> UndoerRef
{
    class UndoOnDelete
    {
        Env env_;
        UndoerRef undoer_;

    public:
        UndoOnDelete(Transaction& t, Selection::Tracker& sc, const IGrid& g):
            env_{ t, sc, g }
        {
        }

        ~UndoOnDelete()
        {
            auto up = Undoer::Param{ env_.diagram(), env_.sel_tracker };
            undoer_.undo(up);
        }

        auto release() -> UndoerRef
        {
            UndoerRef u;
            std::swap(undoer_, u);
            return u;
        }

        void add(UndoerRef u) { undoer_ = combine(undoer_, u); }

        auto getEnv() -> Env& { return env_; }

    } u{ *this, sc, g };

    unsigned int maxrepeat = 10000 + diagram_.getNumOfElements();

    for (;;)
    {
        u.add(subTransactionClose(sc, g));

        if (followUps_.empty())
            break;

        if (not --maxrepeat)
        {
            followUps_.clear();

            class E: public d1::Exception
            {
                const char* what() const { return "Transaction::Close overflow"; }
            };

            throw E{};
        }

        auto f = FollowUpJob{ followUps_.front() };
        followUps_.pop_front();

        f.Do(u.getEnv());
    }

    return u.release();
}


auto C::subTransactionClose(Selection::Tracker& sc, const IGrid& g) -> UndoerRef
{
    if (not imp_)
        return {};

    bool finalize_successful = imp_->finalize(sc, g);
    auto res = imp_->close(sc, g);
    imp_.reset();

    if (finalize_successful)
        return res;

    auto up = Undoer::Param{ diagram_, sc };
    res.undo(up);

    class E: public d1::Exception
    {
        const char* what() const
        {
            return "TransactionImp::Finalize overflow";
        }
    };

    throw E{};
}


void C::abort()
{
    if (imp_)
    {
        imp_->abort();
        imp_.reset();
    }
    followUps_.clear();
}


void C::makeNew()
{
    if (not imp_)
        imp_ = std::make_unique<Imp>(diagram_, workingView_, *this);
}


void C::addTouched(IElement& me, bool update_view)
{
    makeNew();
    D1_ASSERT(imp_.get());
    imp_->addTouched(me, update_view);
}


void C::putIntoTrash(Selection::Tracker& sc, const IElementRef& me)
{
    makeNew();
    D1_ASSERT(imp_.get());
    imp_->putIntoTrash(sc, me);
}


void C::addNewlyCreated(IElementRef me)
{
    makeNew();
    D1_ASSERT(imp_.get());
    imp_->addNewlyCreated(me);
}


bool C::hasNewlyCreated(const IElement& me) const
{
    if (imp_)
        return imp_->hasNewlyCreated(me);

    return false;
}


void C::scheduleFollowUpJob(FollowUpJob f)
{
    d1::push_back_if_missing(followUps_, f);
}

}
