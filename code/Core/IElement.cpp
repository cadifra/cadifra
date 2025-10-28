/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

module Core;

import d1.StackFlag;
import d1.algorithm;

import WinUtil.Debug;


namespace Core
{


#ifdef _DEBUG
namespace
{
bool DebugCreation()
{
    return (WinUtil::DebugEnv::Inst().GetInt("Core", "IElement::DebugCreation") != 0);
}
}
#endif


namespace
{
using C = IElement;
}


class C::Rep
{
public:
    Transaction* itsTransaction = nullptr; // reference only, may be zero
    bool itIsNewlyCreated = false;
    bool itIsTouched = false;
    bool itIsInTrash = false;
    mutable d1::StackFlag::Ref itsViewNeedsUpdateCallIsRunning;
    mutable d1::StackFlag::Ref itsCompletelySelectedCallIsRunning;

    std::vector<std::unique_ptr<IViewElement>> itsViewElements;

    IClub* itsClub = nullptr;

    ~Rep();
};


C::Rep::~Rep()
{
    D1_ASSERT(itsViewElements.empty());
}


C::IElement():
    itsRep{ std::make_unique<Rep>() }
{
#ifdef _DEBUG
    if (DebugCreation())
        WinUtil::dout
            << "New Core::IElement " << this << std::endl;
#endif
}


C::IElement(const IElement& me):
    itsRep{ std::make_unique<Rep>() }
{
#ifdef _DEBUG
    if (DebugCreation())
        WinUtil::dout
            << "New Core::IElement " << this
            << ", copied from " << &me << std::endl;
#endif
}


C::~IElement()
{
    D1_ASSERT(itsRep->itsViewElements.empty());

#ifdef _DEBUG
    if (DebugCreation())
        WinUtil::dout
            << "Destroyed Core::IElement " << this << std::endl;
#endif
}


void C::ViewsNeedUpdate(const IDiagram& d) const
{
    if (itsRep->itsViewNeedsUpdateCallIsRunning)
        return;

    auto stackFlag = d1::StackFlag{ itsRep->itsViewNeedsUpdateCallIsRunning };

    for (const auto& ve : itsRep->itsViewElements)
    {
        D1_ASSERT(ve);
        ve->NeedsUpdate();
    }

    ExtendViewsNeedUpdate(d);
}


auto C::MakeViewElement(IView& v) -> IViewElement*
{
    // lets see, if we already have a view element in v
    auto* ve = ViewElement(&v);

    if (ve)
        return ve; // we do already have a view element in this view

    D1_ASSERT(!ve);

    // ## this model element does not have a view element in v

    auto res = MakeViewElementImp(v);
    ve = res.get();
    if (ve)
    {
        D1_ASSERT(&ve->Element() == this);
        itsRep->itsViewElements.push_back(std::move(res));
    }

    return ve;
}


void C::DeleteViewElement(IViewElement& e)
{
    D1_VERIFY(d1::erase_first_with_get(itsRep->itsViewElements, e));
}


void C::DeleteViewElements(Selection::Tracker& st, IView* v)
{
    for (auto i = begin(itsRep->itsViewElements);
        i != end(itsRep->itsViewElements);
        /* intentionally no ++i here */)
    {
        IViewElement& ve = **i;
        if (v && (&ve.GetView() != v))
        {
            // do not delete
            ++i;
        }
        else
        {
            if (ve.IsSelected())
                st.Changed();
            i = itsRep->itsViewElements.erase(i); // does a ++i !!!!
        }
    }
}


auto C::ViewElement(const IView* theView) const -> IViewElement*
{
    if (!theView)
        return nullptr;

    for (const auto& ve : itsRep->itsViewElements)
    {
        auto& v = ve->GetView();
        if (&v == theView)
            return ve.get();
    }

    return nullptr;
}


void C::PutIntoTrash(Env& e)
{
    if (!itsRep->itIsInTrash)
    {
        auto sp = this->shared_from_this();
        auto me = std::dynamic_pointer_cast<IElement>(sp);
        e.transaction.PutIntoTrash(e.sel_tracker, me);
    }
}


void C::Finalize(Env& e)
{
    // by default: nothing to finalize
}


void C::DisconnectTransaction()
{
    itsRep->itsTransaction = 0;
    itsRep->itIsNewlyCreated = false;
    itsRep->itIsTouched = false;

    Lock();
    TransactionDone();

    for (const auto& v : itsRep->itsViewElements)
    {
        D1_ASSERT(v);
        v->TransactionDone();
    }
}


void C::SetSelectionState(Selection::Tracker& sc, bool new_state, IView& v)
{
    auto* ve = ViewElement(&v);
    if (!ve)
        return;

    auto sve = ve->Selectable();
    if (!sve)
        return;

    sve->SetSelectionState(sc, new_state);
}


bool C::IsSelected(const IView* v) const
{
    auto* ve = ViewElement(v);
    if (!ve)
        return false;

    auto sve = ve->Selectable();
    if (!sve)
        return false;

    return sve->IsSelected();
}


bool C::IsCompletelySelected(const ElementSet& selection) const
{
    if (itsRep->itsCompletelySelectedCallIsRunning)
    {
        // recursion detected: this can happen with recursive data structures
        // created by the user (absolutely legal).
        // We have been thinking quite some time what we should do in this
        // situation here. Finally, Frank had the idea just to say that in
        // this case here we are completely selected. Basically, if we
        // depend about ourselves whether to be completely selected or not,
        // we say just "yes" here. If, for some model element, there are other
        // stronger checks, we just let those other checks decide.
        return true;
    }

    auto stackFlag = d1::StackFlag{ itsRep->itsCompletelySelectedCallIsRunning };

    return IsCompletelySelectedImp(selection);
}


bool C::IsCompletelySelectedImp(const ElementSet& selection) const
{
    // ## Default implementation. Often not appropriate.
    return selection.Contains(*this);
}


auto C::IsCopySelected(ExtendSelection::Param& p) const
    -> ExtendSelection::Result
{
    // ## Default implementation.
    return p.Selection().Contains(*this)
               ? ExtendSelection::Result::yes
               : ExtendSelection::Result::no;
}


auto C::GetTransaction() const -> Transaction*
{
    return itsRep->itsTransaction;
}


bool C::IsInTransaction() const
{
    return itsRep->itsTransaction != 0;
}


bool C::IsNewlyCreated() const
{
    return itsRep->itIsNewlyCreated;
}


bool C::IsTouched() const
{
    return itsRep->itIsTouched;
}


bool C::IsInTrash() const
{
    return itsRep->itIsInTrash;
}


void C::SetInTrashFlag(bool f)
{
    itsRep->itIsInTrash = f;
}


void C::SetTransaction(Transaction* t)
{
    itsRep->itsTransaction = t;
}


void C::SetNewlyCreated(bool nc)
{
    itsRep->itIsNewlyCreated = nc;
}


void C::SetTouched(bool t)
{
    itsRep->itIsTouched = t;
}


auto C::MakeViewElementImp(IView& v) -> std::unique_ptr<IViewElement>
{
    return nullptr;
}


void C::SetClub(IClub* c)
{
    itsRep->itsClub = c;
}


IClub* C::Club() const
{
    return itsRep->itsClub;
}


void C::ExtendViewsNeedUpdate(const IDiagram&) const
{
}


void C::DeepInsert(ElementSet& s)
{
    if (IsInTrash())
        return;
    s.Insert(*this);
}


}
