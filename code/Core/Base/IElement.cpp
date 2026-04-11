/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

module Core:_;
import :Base;

import :Transaction;
import :View;

import d1.StackFlag;
import d1.algorithm;

import WinUtil.Debug;


namespace Core
{


#ifdef _DEBUG
namespace
{
bool debugCreation()
{
    return (WinUtil::DebugEnv::inst().getInt("Core", "IElement::DebugCreation") != 0);
}
}
#endif


namespace
{
using C = IElement;
}


struct C::Rep
{
    Transaction* transaction = nullptr; // reference only, may be zero
    bool newlyCreated = false;
    bool touched = false;
    bool inTrash = false;
    mutable d1::StackFlag::Ref viewNeedsUpdateCallIsRunning;
    mutable d1::StackFlag::Ref completelySelectedCallIsRunning;

    std::vector<std::unique_ptr<IViewElement>> viewElements;

    IClub* club = nullptr;

    ~Rep();
};


C::Rep::~Rep()
{
    D1_ASSERT(viewElements.empty());
}


C::IElement():
    rep_{ std::make_unique<Rep>() }
{
#ifdef _DEBUG
    if (debugCreation())
        WinUtil::dout
            << "New Core::IElement " << this << std::endl;
#endif
}


C::IElement(const IElement& me):
    rep_{ std::make_unique<Rep>() }
{
#ifdef _DEBUG
    if (debugCreation())
        WinUtil::dout
            << "New Core::IElement " << this
            << ", copied from " << &me << std::endl;
#endif
}


C::~IElement()
{
    D1_ASSERT(rep_->viewElements.empty());

#ifdef _DEBUG
    if (debugCreation())
        WinUtil::dout
            << "Destroyed Core::IElement " << this << std::endl;
#endif
}


void C::viewsNeedUpdate(const IDiagram& d) const
{
    if (rep_->viewNeedsUpdateCallIsRunning)
        return;

    auto stackFlag = d1::StackFlag{ rep_->viewNeedsUpdateCallIsRunning };

    for (const auto& ve : rep_->viewElements)
    {
        D1_ASSERT(ve);
        ve->needsUpdate();
    }

    extendViewsNeedUpdate(d);
}


auto C::makeViewElement(IView& v) -> IViewElement*
{
    // lets see, if we already have a view element in v
    auto* ve = viewElement(&v);

    if (ve)
        return ve; // we do already have a view element in this view

    D1_ASSERT(not ve);

    // ## this model element does not have a view element in v

    auto res = makeViewElementImp(v);
    ve = res.get();
    if (ve)
    {
        D1_ASSERT(&ve->element() == this);
        rep_->viewElements.push_back(std::move(res));
    }

    return ve;
}


void C::Delete(IViewElement& e)
{
    D1_VERIFY(d1::erase_first_with_get(rep_->viewElements, e));
}


void C::deleteViewElements(Selection::Tracker& st, IView* v)
{
    for (auto i = begin(rep_->viewElements);
        i != end(rep_->viewElements);
        /* intentionally no ++i here */)
    {
        IViewElement& ve = **i;
        if (v and (&ve.getView() != v))
        {
            // do not delete
            ++i;
        }
        else
        {
            if (ve.isSelected())
                st.changed();
            i = rep_->viewElements.erase(i);
        }
    }
}


auto C::viewElement(const IView* theView) const -> IViewElement*
{
    if (not theView)
        return nullptr;

    for (const auto& ve : rep_->viewElements)
    {
        auto& v = ve->getView();
        if (&v == theView)
            return ve.get();
    }

    return nullptr;
}


void C::putIntoTrash(Env& e)
{
    if (not rep_->inTrash)
    {
        auto sp = this->shared_from_this();
        auto me = std::dynamic_pointer_cast<IElement>(sp);
        e.transaction.putIntoTrash(e.sel_tracker, me);
    }
}


void C::finalize(Env& e)
{
    // by default: nothing to finalize
}


void C::disconnectTransaction()
{
    rep_->transaction = 0;
    rep_->newlyCreated = false;
    rep_->touched = false;

    lock();
    transactionDone();

    for (const auto& v : rep_->viewElements)
    {
        D1_ASSERT(v);
        v->transactionDone();
    }
}


void C::setSelectionState(Selection::Tracker& sc, bool new_state, IView& v)
{
    auto* ve = viewElement(&v);
    if (not ve)
        return;

    auto sve = ve->selectable();
    if (not sve)
        return;

    sve->setSelectionState(sc, new_state);
}


bool C::isSelected(const IView* v) const
{
    auto* ve = viewElement(v);
    if (not ve)
        return false;

    auto sve = ve->selectable();
    if (not sve)
        return false;

    return sve->isSelected();
}


bool C::isCompletelySelected(const ElementSet& selection) const
{
    if (rep_->completelySelectedCallIsRunning)
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

    auto stackFlag = d1::StackFlag{ rep_->completelySelectedCallIsRunning };

    return isCompletelySelectedImp(selection);
}


bool C::isCompletelySelectedImp(const ElementSet& selection) const
{
    // ## Default implementation. Often not appropriate.
    return selection.contains(*this);
}


auto C::isCopySelected(ExtendSelection::Param& p) const
    -> ExtendSelection::Result
{
    // ## Default implementation.
    return p.selection().contains(*this)
               ? ExtendSelection::Result::yes
               : ExtendSelection::Result::no;
}


auto C::getTransaction() const -> Transaction*
{
    return rep_->transaction;
}


bool C::isInTransaction() const
{
    return rep_->transaction != 0;
}


bool C::isNewlyCreated() const
{
    return rep_->newlyCreated;
}


bool C::isTouched() const
{
    return rep_->touched;
}


bool C::isInTrash() const
{
    return rep_->inTrash;
}


void C::setInTrashFlag(bool f)
{
    rep_->inTrash = f;
}


void C::set(Transaction* t)
{
    rep_->transaction = t;
}


void C::setNewlyCreated(bool nc)
{
    rep_->newlyCreated = nc;
}


void C::setTouched(bool t)
{
    rep_->touched = t;
}


auto C::makeViewElementImp(IView& v) -> std::unique_ptr<IViewElement>
{
    return nullptr;
}


void C::setClub(IClub* c)
{
    rep_->club = c;
}


IClub* C::club() const
{
    return rep_->club;
}


void C::extendViewsNeedUpdate(const IDiagram&) const
{
}


void C::deepInsert(ElementSet& s)
{
    if (isInTrash())
        return;
    s.insert(*this);
}


}
