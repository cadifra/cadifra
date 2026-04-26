/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core.Main;

import d1.CastIteratorImp;
import d1.algorithm;


namespace Core
{

namespace
{
using C = TransferSet;
}


C::~TransferSet()
{
    clear();
}


void C::add(IElementRef m)
{
    elements_.push_back(m);
    m->setClub(this);
}


void C::remove(IElement& m)
{
    d1::erase_first_with_get(elements_, m);
}


void C::setFocus(ObjectID id)
{
    focus_ = 0;

    for (auto mi : elements_)
    {
        auto& me = *mi.get();
        if (me.getID() == id)
        {
            focus_ = &me;
            return;
        }
    }
}


void C::clear()
{
    elements_.clear();
    focus_ = 0;
}


auto C::paste(Transaction& t) const -> PasteRes
{
    PasteRes res;

    D1_ASSERT(not copier_); // paste is not allowed while there is an active copier

    if (elements_.empty())
        return res; // nothing to paste

    auto r = CopyRegistry::makeNew();

    MeSet copies;

    // first pass: create all copies --------------------------------------------

    for (auto mi : elements_)
    {
        const auto& orig = *mi.get();

        if (not orig.acceptForPaste(t.diagram()))
            continue;

        auto copy{ orig.copy() };

        copies.push_back(copy);
        r->addMapping(&orig, copy.get());

        if (&orig == focus_)
            res.focus = copy.get();
    }

    // ## copies may not be used at all because their refs are not adjusted yet!!
    // ## WARNING: do not do anything with the copies until their refs have been
    // ##          adjusted).

    // second pass: adjust pointers of copies ------------------------------------

    for (auto mi : copies)
    {
        auto copy = mi.get();
        D1_ASSERT(copy);
        copy->adjustRefsAfterCopy(*r);
    }

    // ## copies are consistent now and may be used
    for (auto mi : copies)
        t.addNewlyCreated(mi);

    for (auto mi : copies)
        res.pasted_elements.insert(*mi.get());

    return res;
}


void C::assignIDs()
{
    ObjectID id;

    for (auto mi : elements_)
    {
        auto me = mi.get();
        me->setID(++id);
    }
}


auto C::getGroup() -> Group&
{
    class GroupImp: public Group
    {
        TransferSet& ts_;

        using CastIter =
            d1::CastIteratorImp<
                d1::Iterator<IElement*>, IElement*, IElement*>;

        using SI = d1::Iterator<IElement*>;

    public:
        GroupImp(TransferSet& ts):
            ts_{ ts }
        {
        }

        void assignIDs() final { ts_.assignIDs(); };

        SI begin() const final
        {
            return { std::make_unique<CastIter>(ts_.begin()) };
        }

        SI end() const final
        {
            return { std::make_unique<CastIter>(ts_.end()) };
        }
    };

    if (not group_)
        group_ = std::make_unique<GroupImp>(*this);

    return *group_.get();
}


auto C::startCopy() -> Copier
{
    clear();
    return { *this };
}


namespace
{
class IteratorImp: public d1::Iterator<IElement*>::Imp
{
    using Base = d1::Iterator<IElement*>::Imp;
    using C = TransferSet::MeSet;

    C::const_iterator iterator_;

    auto clone() const -> std::unique_ptr<Base> final
    {
        return std::make_unique<IteratorImp>(iterator_);
    }

    bool isEqual(const Base& rhs) const final
    {
        return iterator_ ==
               static_cast<const IteratorImp&>(rhs).iterator_;
    }

    void next() final { ++iterator_; }

    value_type val() const final { return iterator_->get(); }

public:
    IteratorImp(C::const_iterator i):
        iterator_{ i }
    {
    }
};
}


auto C::begin() const -> d1::Iterator<IElement*>
{
    return { std::make_unique<IteratorImp>(elements_.begin()) };
}


auto C::end() const -> d1::Iterator<IElement*>
{
    return { std::make_unique<IteratorImp>(elements_.end()) };
}


C::Copier::Copier(TransferSet& c):
    transferSet_{ c },
    copyRegistry_{ nullptr }
{
    D1_ASSERT(not transferSet_.copier_);
    transferSet_.copier_ = this;
}


auto C::Copier::addCopy(IElement& m) -> IElement&
{
    if (not copyRegistry_)
        copyRegistry_ = CopyRegistry::makeNew();

    D1_ASSERT(copyRegistry_);
    D1_ASSERT(transferSet_.copier_ == this);

    auto copy{ m.copy() };
    transferSet_.add(copy);

    copyRegistry_->addMapping(/*original*/ &m, /*copy*/ copy.get());

    return *copy.get();
}



void C::Copier::addFocusCopy(IElement& m)
{
    transferSet_.focus_ = &addCopy(m);
}


void C::Copier::complete()
{
    if (not copyRegistry_)
        return;

    D1_ASSERT(copyRegistry_);
    D1_ASSERT(transferSet_.copier_ == this);

    for (auto mi : transferSet_.elements_)
    {
        auto& me = *mi.get();
        me.adjustRefsAfterCopy(*copyRegistry_);
    }

    transferSet_.copier_ = 0;

    copyRegistry_.reset();
}

}
