/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

import :UndoerImp;

import d1.algorithm;
import d1.Point;

import WinUtil.Debug;
import WinUtil.GuardedFunctionCall;


namespace Core
{

using std::ranges::for_each;


#ifdef _DEBUG
namespace
{
int debugOutputEnabled()
{
    return WinUtil::DebugEnv::inst().getInt("Core", "Transaction::Debug");
}
}
#endif


namespace
{
d1::uint32 GlobalTransactionCounter = 0;
}

namespace
{
using C = Transaction;
}

C::Imp::Imp(
    IDiagram& d, IView* working_view, Transaction& trans):

    transaction_{ trans },
    diagram_{ d },
    workingView_{ working_view },
    number_{ ++GlobalTransactionCounter }
{
#ifdef _DEBUG
    if (debugOutputEnabled())
        WinUtil::dout
            << std::endl
            << "------- New Transaction "
            << GlobalTransactionCounter << std::endl;
#endif

    diagram_.add(transaction_);
}


void C::Imp::destruct()
{
    diagram_.forget(transaction_);

    class DiagramUpdater
    {
        IDiagram& diagram_;

    public:
        DiagramUpdater(IDiagram& d):
            diagram_{ d }
        {
        }
        ~DiagramUpdater() { diagram_.updateViews(); }

    } diagramUpdater{ diagram_ };

    if (not closed_)
    {
#ifdef _DEBUG
        if (debugOutputEnabled())
            WinUtil::dout
                << std::endl
                << "******* Warning: Forcing closure of Transaction "
                << GlobalTransactionCounter << std::endl;
#endif
        abort();
    }
}


C::Imp::~Imp()
{
    WinUtil::GuardedCallHelpers::call(
        "Core::TransactionImp::~TransactionImp", *this, &Imp::destruct);
}


bool C::Imp::finalize(Selection::Tracker& sc, const IGrid& g)
{
    auto e = Env{ transaction_, sc, g };
    // warning: *this may be changed through e

    unsigned int maxrepeat = 100000;

    Finalizer::getDock().executeAll(e);

    while (not finalizeCandidates_.empty())
    {
        if (not --maxrepeat)
        {
            finalizeCandidates_.clear();
            return false;
        }

        IElement* me = finalizeCandidates_.front().get();
        D1_ASSERT(me);

        me->finalize(e);
        d1::erase_first_with_get(finalizeCandidates_, *me);
    }

    return true;
}


auto C::Imp::close(Selection::Tracker& sc, const IGrid& g) -> UndoerRef
{
    D1_ASSERT(finalizeCandidates_.empty());

    bool null_transaction = false;
    UndoerRef res;

    if (touchedElements_.empty() and
        newlyCreatedElements_.empty() and
        trashedElements_.empty())
    {
        null_transaction = true;
    }
    else
    {
        null_transaction = false;

        res = nonNullClose(sc, g);
    }

#ifdef _DEBUG
    if (debugOutputEnabled())
    {
        WinUtil::dout
            << "------- " << (null_transaction ? "empty " : "") << "Transaction "
            << GlobalTransactionCounter << " closed. " << std::endl;
    }
#endif

    closed_ = true;

    return res;
}


auto C::Imp::nonNullClose(Selection::Tracker& sc, const IGrid& g) -> UndoerRef
{
    auto res = closeAllTouched();

    disconnectAllNewlyCreated();
    disconnectAllTouched();
    disconnectAllTrashed(sc);

    touchedElements_.clear();

    toBeDeletedElements_.clear();

    return res;
}


namespace
{

class ConvertPosUndoers
{
    using V = std::vector<UndoerRef>;
    using I = V::const_iterator;
    V v_;

public:
    ConvertPosUndoers() {}

    bool add(UndoerRef u)
    {
        if (u.get()->getPosUndoer())
        {
            v_.push_back(u);
            return true;
        }
        return false;
    }

    void getResult(SequenceUndoer& su);
    void getSameOffset(const d1::Vector& offset, SequenceUndoer& su);
};


void ConvertPosUndoers::getSameOffset(const d1::Vector& offset, SequenceUndoer& su)
{
    auto other_offset = V{};
    auto matched_offset = V{};

    for (auto iu : v_)
    {
        auto* p = iu.get()->getPosUndoer();
        if (p->offset() == offset)
            matched_offset.push_back(iu);
        else
            other_offset.push_back(iu);
    }

    v_.swap(other_offset);

    if (matched_offset.size() == 1)
    {
        su.append(matched_offset.front());
        return;
    }

    auto pug = std::make_shared<PosUndoerGroup>(offset);

    for (auto iu : matched_offset)
    {
        auto* p = iu.get()->getPosUndoer();
        pug->add(p->object());
    }

    su.append({ pug });
}


void ConvertPosUndoers::getResult(SequenceUndoer& su)
{
    while (not v_.empty())
    {
        auto* p = v_.front().get()->getPosUndoer();
        getSameOffset(p->offset(), su);
    }
}

}


auto C::Imp::closeAllTouched() -> UndoerRef
{
    class UColl: public IUndoerCollector
    {
        SequenceUndoer* su_ = nullptr; // no ownership
        UndoerRef su_ref_;
        ConvertPosUndoers cpu_;

    public:
        UColl()
        {
            auto su = std::make_shared<SequenceUndoer>();
            su_ = su.get();
            su_ref_ = { su };
        }

        void add(UndoerRef u)
        {
            if (cpu_.add(u))
                return;
            su_->append(u);
        }

        auto getResult() -> UndoerRef
        {
            cpu_.getResult(*su_);
            return su_ref_;
        }
    } uc;

    for (auto im : touchedElements_)
    {
        IElement* me = im.get();
        D1_ASSERT(me);
        me->transactionClosed(uc);
    }

    auto su = std::make_shared<TransactionUndoer>(
        uc.getResult(), newlyCreatedElements_, trashedElements_, number_);

    return { su };
}


void C::Imp::disconnectAllTouched()
{
    for (auto im : touchedElements_)
    {
        IElement& me = *im.get();
        me.viewsNeedUpdate(diagram_);
        me.disconnectTransaction();
    }
}


void C::Imp::disconnectAllTrashed(Selection::Tracker& sc)
{
    for (auto im : trashedElements_)
    {
        IElement& me = *im.get();
        diagram_.destroyViewElements(me, sc);
        me.disconnectTransaction();
    }
}


void C::Imp::disconnectAllNewlyCreated()
{
    for (auto im : newlyCreatedElements_)
    {
        IElement& me = *im.get();

        // create view elements for newly created clients in all non-working views
        diagram_.createViewElements(me);
        me.viewsNeedUpdate(diagram_);
        me.disconnectTransaction();
    }
}


void C::Imp::abort()
{
    finalizeCandidates_.clear();
    auto sc = Selection::Tracker{ workingView_ };

    for_each(touchedElements_,
        [](const IElementRef& me) {
            IElement::PrivateAccess::transactionAborted(*me.get());
        });

    abortAllTrashed();

    abortAllNewlyCreated(sc);

    auto duv = [&](const IElementRef& me) {
        IElement::PrivateAccess::disconnectTransaction(*me.get());
        me->viewsNeedUpdate(diagram_);
    };

    for_each(trashedElements_, duv);
    for_each(touchedElements_, duv);

    touchedElements_.clear();
    newlyCreatedElements_.clear();
    toBeDeletedElements_.clear();
    trashedElements_.clear();
    closed_ = true;
}


void C::Imp::abortAllNewlyCreated(Selection::Tracker& sc)
{
    for (auto im : newlyCreatedElements_)
    {
        IElement& me = *im.get();
        diagram_.destroyViewElements(me, sc);
        diagram_.remove(me);
        me.disconnectTransaction();
    }
}


void C::Imp::abortAllTrashed()
{
    for (const auto& im : trashedElements_)
    {
        IElement& me = *im.get();
        me.transactionAborted();
        me.setInTrashFlag(false);
        diagram_.insert(im);
        diagram_.createViewElements(me, false);
    }
}


namespace
{
template <class C>
bool notContains_(const C& cont, const IElement* ele)
{
    auto i = std::ranges::find_if(
        cont,
        [=](auto& m) {
            return (m.get() == ele);
        });
    return (i == end(cont));
}
template <class C>
bool contains_(const C& cont, const IElement* ele)
{
    return not notContains_(cont, ele);
}
}


void C::Imp::addTouched(IElement& me, bool update_view)
{
    // some checks first

    D1_ASSERT(not closed_);
    D1_ASSERT(not trashedElements_.contains(me));

    const bool touched = me.isTouched();
    const bool newly_created = me.isNewlyCreated();

    if (touched or newly_created)
    {
        D1_ASSERT(me.getTransaction() == &transaction_);
    }

    if (newly_created)
    {
        D1_ASSERT(not touched);
        D1_ASSERT(contains_(newlyCreatedElements_, &me));
    }
    else
    {
        D1_ASSERT(notContains_(newlyCreatedElements_, &me));
    }

    if (touched)
    {
        D1_ASSERT(not newly_created);
        D1_ASSERT(contains_(touchedElements_, &me));
    }
    else
    {
        D1_ASSERT(notContains_(touchedElements_, &me));
    }

    // ## checks passed

    auto sp = me.shared_from_this();
    auto sme = std::dynamic_pointer_cast<IElement>(sp);

    if (not touched and not newly_created)
    {
        me.set(&transaction_);
        touchedElements_.push_back(sme);
        D1_ASSERT(me.getTransaction() == &transaction_);
        me.setTouched(true);
    }

    if (notContains_(finalizeCandidates_, &me))
        finalizeCandidates_.push_back(sme);

    if (update_view)
        me.viewsNeedUpdate(diagram_);
}


void C::Imp::putIntoTrash(Selection::Tracker& sc, const IElementRef& me)
{
    D1_ASSERT(not closed_);

    const bool newly_created = me->isNewlyCreated();

    d1::erase_first(finalizeCandidates_, me);

    if (newly_created)
    {
        toBeDeletedElements_.push_back(me);
        d1::erase_first(newlyCreatedElements_, me);
    }
    else
    {
        trashedElements_.insert(*me.get());
    }

    me->setInTrashFlag(true);

    diagram_.remove(*me.get());

    if (newly_created)
        diagram_.destroyViewElements(*me.get(), sc);
    else
        diagram_.destroyViewElements(*me.get(), sc, workingView_);

#ifdef _DEBUG
    if (debugOutputEnabled() >= 2)
        WinUtil::dout
            << "      DEL " << typeid(me).name() << " "
            << &me
            << std::endl;
#endif
}


void C::Imp::addNewlyCreated(IElementRef me)
{
    D1_ASSERT(not closed_);

    D1_ASSERT(notContains_(touchedElements_, me.get()));
    D1_ASSERT(notContains_(trashedElements_, me.get()));
    D1_ASSERT(notContains_(newlyCreatedElements_, me.get()));

    D1_ASSERT(me->getTransaction() == 0);
    D1_ASSERT(not me->isTouched());
    D1_ASSERT(not me->isNewlyCreated());

    newlyCreatedElements_.push_back(me);
    diagram_.insert(me);
    diagram_.createViewElements(*me.get(), false);

    me->setNewlyCreated(true);
    me->set(&transaction_);

    finalizeCandidates_.push_back(me);

#ifdef _DEBUG
    if (debugOutputEnabled() >= 2)
        WinUtil::dout
            << "NEW " << typeid(*me.get()).name() << " "
            << me.get()
            << std::endl;
#endif
}


bool C::Imp::hasNewlyCreated(const IElement& me) const
{
    D1_ASSERT(not closed_);
    return not notContains_(newlyCreatedElements_, &me);
}

}
