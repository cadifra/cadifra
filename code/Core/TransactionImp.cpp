/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

import d1.algorithm;
import d1.Point;

import WinUtil;


namespace Core
{

using std::ranges::for_each;


#ifdef _DEBUG
namespace
{
int DebugOutputEnabled()
{
    return WinUtil::DebugEnv::Inst().GetInt("Core", "Transaction::Debug");
}
}
#endif


namespace
{
d1::uint32 GlobalTransactionCounter = 0;
}


TransactionImp::TransactionImp(
    IDiagram& d, IView* working_view, Transaction& trans):

    itsTransaction{ trans },
    itsDiagram{ d },
    itsWorkingView{ working_view },
    itsNumber{ ++GlobalTransactionCounter }
{
#ifdef _DEBUG
    if (DebugOutputEnabled())
        WinUtil::dout
            << std::endl
            << "------- New Transaction "
            << GlobalTransactionCounter << std::endl;
#endif

    itsDiagram.RegisterTransaction(itsTransaction);
}


namespace
{
using C = TransactionImp;
}


void C::Destruct()
{
    itsDiagram.UnregisterTransaction(itsTransaction);

    class DiagramUpdater
    {
        IDiagram& itsDiagram;

    public:
        DiagramUpdater(IDiagram& d):
            itsDiagram{ d }
        {
        }
        ~DiagramUpdater() { itsDiagram.UpdateViews(); }

    } diagramUpdater{ itsDiagram };

    if (!itIsClosed)
    {
#ifdef _DEBUG
        if (DebugOutputEnabled())
            WinUtil::dout
                << std::endl
                << "******* Warning: Forcing closure of Transaction "
                << GlobalTransactionCounter << std::endl;
#endif
        Abort();
    }
}


C::~TransactionImp()
{
    WinUtil::GuardedCallHelpers::call(
        "Core::TransactionImp::~TransactionImp", *this, &TransactionImp::Destruct);
}


bool C::Finalize(SelectionTracker& sc, const IGrid& g)
{
    auto e = Env{ itsTransaction, sc, g };
    // warning: *this may be changed through e

    unsigned int maxrepeat = 100000;

    FinalizerDock::Get().ExecuteAll(e);

    while (!itsFinalizeCandidates.empty())
    {
        if (!--maxrepeat)
        {
            itsFinalizeCandidates.clear();
            return false;
        }

        IElement* me = itsFinalizeCandidates.front().get();
        D1_ASSERT(me);

        me->Finalize(e);
        d1::erase_first_with_get(itsFinalizeCandidates, *me);
    }

    return true;
}


auto C::Close(SelectionTracker& sc, const IGrid& g) -> UndoerRef
{
    D1_ASSERT(itsFinalizeCandidates.empty());

    bool null_transaction = false;
    UndoerRef res;

    if (itsTouchedElements.empty() &&
        itsNewlyCreatedElements.empty() &&
        itsTrashedElements.empty())
    {
        null_transaction = true;
    }
    else
    {
        null_transaction = false;

        res = NonNullClose(sc, g);
    }

#ifdef _DEBUG
    if (DebugOutputEnabled())
    {
        WinUtil::dout
            << "------- " << (null_transaction ? "empty " : "") << "Transaction "
            << GlobalTransactionCounter << " closed. " << std::endl;
    }
#endif

    itIsClosed = true;

    return res;
}


auto C::NonNullClose(SelectionTracker& sc, const IGrid& g) -> UndoerRef
{
    auto res = CloseAllTouched();

    DisconnectAllNewlyCreated();
    DisconnectAllTouched();
    DisconnectAllTrashed(sc);

    itsTouchedElements.clear();

    itsToBeDeletedElements.clear();

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

    bool Add(UndoerRef u)
    {
        if (u.get()->GetPosUndoer())
        {
            v_.push_back(u);
            return true;
        }
        return false;
    }

    void GetResult(SequenceUndoer& su);
    void GetSameOffset(const d1::Vector& offset, SequenceUndoer& su);
};


void ConvertPosUndoers::GetSameOffset(const d1::Vector& offset, SequenceUndoer& su)
{
    auto other_offset = V{};
    auto matched_offset = V{};

    for (auto iu : v_)
    {
        auto* p = iu.get()->GetPosUndoer();
        if (p->Offset() == offset)
            matched_offset.push_back(iu);
        else
            other_offset.push_back(iu);
    }

    v_.swap(other_offset);

    if (matched_offset.size() == 1)
    {
        su.Append(matched_offset.front());
        return;
    }

    auto pug = std::make_shared<PosUndoerGroup>(offset);

    for (auto iu : matched_offset)
    {
        auto* p = iu.get()->GetPosUndoer();
        pug->Add(p->Object());
    }

    su.Append({ pug });
}


void ConvertPosUndoers::GetResult(SequenceUndoer& su)
{
    while (!v_.empty())
    {
        auto* p = v_.front().get()->GetPosUndoer();
        GetSameOffset(p->Offset(), su);
    }
}

}


auto C::CloseAllTouched() -> UndoerRef
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

        void Add(UndoerRef u)
        {
            if (cpu_.Add(u))
                return;
            su_->Append(u);
        }

        auto GetResult() -> UndoerRef
        {
            cpu_.GetResult(*su_);
            return su_ref_;
        }
    } uc;

    for (auto im : itsTouchedElements)
    {
        IElement* me = im.get();
        D1_ASSERT(me);
        me->TransactionClosed(uc);
    }

    auto su = std::make_shared<TransactionUndoer>(
        uc.GetResult(), itsNewlyCreatedElements, itsTrashedElements, itsNumber);

    return { su };
}


void C::DisconnectAllTouched()
{
    for (auto im : itsTouchedElements)
    {
        IElement& me = *im.get();
        me.ViewsNeedUpdate(itsDiagram);
        me.DisconnectTransaction();
    }
}


void C::DisconnectAllTrashed(SelectionTracker& sc)
{
    for (auto im : itsTrashedElements)
    {
        IElement& me = *im.get();
        itsDiagram.DestroyViewElements(me, sc);
        me.DisconnectTransaction();
    }
}


void C::DisconnectAllNewlyCreated()
{
    for (auto im : itsNewlyCreatedElements)
    {
        IElement& me = *im.get();

        // create view elements for newly created clients in all non-working views
        itsDiagram.CreateViewElements(me);
        me.ViewsNeedUpdate(itsDiagram);
        me.DisconnectTransaction();
    }
}


void C::Abort()
{
    itsFinalizeCandidates.clear();
    auto sc = SelectionTracker{ itsWorkingView };

    for_each(itsTouchedElements,
        [](const IElementPtr& me) {
            IElementPrivateAccess::TransactionAborted(*me.get());
        });

    AbortAllTrashed();

    AbortAllNewlyCreated(sc);

    auto duv = [&](const IElementPtr& me) {
        IElementPrivateAccess::DisconnectTransaction(*me.get());
        me->ViewsNeedUpdate(itsDiagram);
    };

    for_each(itsTrashedElements, duv);
    for_each(itsTouchedElements, duv);

    itsTouchedElements.clear();
    itsNewlyCreatedElements.clear();
    itsToBeDeletedElements.clear();
    itsTrashedElements.clear();
    itIsClosed = true;
}


void C::AbortAllNewlyCreated(SelectionTracker& sc)
{
    for (auto im : itsNewlyCreatedElements)
    {
        IElement& me = *im.get();
        itsDiagram.DestroyViewElements(me, sc);
        itsDiagram.Remove(me);
        me.DisconnectTransaction();
    }
}


void C::AbortAllTrashed()
{
    for (const auto& im : itsTrashedElements)
    {
        IElement& me = *im.get();
        me.TransactionAborted();
        me.SetInTrashFlag(false);
        itsDiagram.Insert(im);
        itsDiagram.CreateViewElements(me, false);
    }
}


namespace
{
template <class C>
bool NotContains_(const C& cont, const IElement* ele)
{
    auto i = std::find_if(
        begin(cont), end(cont),
        [=](auto& m) {
            return (m.get() == ele);
        });
    return (i == end(cont));
}
template <class C>
bool Contains_(const C& cont, const IElement* ele)
{
    return !NotContains_(cont, ele);
}
}


void C::AddTouched(IElement& me, bool update_view)
{
    // some checks first

    D1_ASSERT(!itIsClosed);
    D1_ASSERT(!itsTrashedElements.Contains(me));

    const bool touched = me.IsTouched();
    const bool newly_created = me.IsNewlyCreated();

    if (touched || newly_created)
    {
        D1_ASSERT(me.GetTransaction() == &itsTransaction);
    }

    if (newly_created)
    {
        D1_ASSERT(!touched);
        D1_ASSERT(Contains_(itsNewlyCreatedElements, &me));
    }
    else
    {
        D1_ASSERT(NotContains_(itsNewlyCreatedElements, &me));
    }

    if (touched)
    {
        D1_ASSERT(!newly_created);
        D1_ASSERT(Contains_(itsTouchedElements, &me));
    }
    else
    {
        D1_ASSERT(NotContains_(itsTouchedElements, &me));
    }

    // ## checks passed

    auto sp = me.shared_from_this();
    auto sme = std::dynamic_pointer_cast<IElement>(sp);

    if (!touched && !newly_created)
    {
        me.SetTransaction(&itsTransaction);
        itsTouchedElements.push_back(sme);
        D1_ASSERT(me.GetTransaction() == &itsTransaction);
        me.SetTouched(true);
    }

    if (NotContains_(itsFinalizeCandidates, &me))
        itsFinalizeCandidates.push_back(sme);

    if (update_view)
        me.ViewsNeedUpdate(itsDiagram);
}


void C::PutIntoTrash(SelectionTracker& sc, const IElementPtr& me)
{
    D1_ASSERT(!itIsClosed);

    const bool newly_created = me->IsNewlyCreated();

    d1::erase_first(itsFinalizeCandidates, me);

    if (newly_created)
    {
        itsToBeDeletedElements.push_back(me);
        d1::erase_first(itsNewlyCreatedElements, me);
    }
    else
    {
        itsTrashedElements.Insert(*me.get());
    }

    me->SetInTrashFlag(true);

    itsDiagram.Remove(*me.get());

    if (newly_created)
        itsDiagram.DestroyViewElements(*me.get(), sc);
    else
        itsDiagram.DestroyViewElements(*me.get(), sc, itsWorkingView);

#ifdef _DEBUG
    if (DebugOutputEnabled() >= 2)
        WinUtil::dout
            << "      DEL " << typeid(me).name() << " "
            << &me
            << std::endl;
#endif
}


void C::AddNewlyCreated(IElementPtr me)
{
    D1_ASSERT(!itIsClosed);

    D1_ASSERT(NotContains_(itsTouchedElements, me.get()));
    D1_ASSERT(NotContains_(itsTrashedElements, me.get()));
    D1_ASSERT(NotContains_(itsNewlyCreatedElements, me.get()));

    D1_ASSERT(me->GetTransaction() == 0);
    D1_ASSERT(!me->IsTouched());
    D1_ASSERT(!me->IsNewlyCreated());

    itsNewlyCreatedElements.push_back(me);
    itsDiagram.Insert(me);
    itsDiagram.CreateViewElements(*me.get(), false);

    me->SetNewlyCreated(true);
    me->SetTransaction(&itsTransaction);

    itsFinalizeCandidates.push_back(me);

#ifdef _DEBUG
    if (DebugOutputEnabled() >= 2)
        WinUtil::dout
            << "NEW " << typeid(*me.get()).name() << " "
            << me.get()
            << std::endl;
#endif
}


bool C::HasNewlyCreated(const IElement& me) const
{
    D1_ASSERT(!itIsClosed);
    return !NotContains_(itsNewlyCreatedElements, &me);
}

}
