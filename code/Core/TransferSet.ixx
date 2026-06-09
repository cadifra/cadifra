/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.TransferSet;

import Core.Main;
import Core.ObjectID;

import std;


namespace Core
{

export class TransferSet: public IClub
{
public:
    class Copier;

    using MeSet = std::vector<IElementRef>;

private:
    MeSet elements_;
    IElement* focus_ = nullptr; // one of elements_. ref only
    Copier* copier_ = nullptr;  // ref only. May be zero

    std::unique_ptr<Group> group_;

public:
    //-- Core::IClub

    auto begin() const -> d1::Iterator<IElement*> override;
    auto end() const -> d1::Iterator<IElement*> override;

    //--

    TransferSet() = default;

    TransferSet(const TransferSet&) = delete;
    TransferSet& operator=(const TransferSet&) = delete;

    virtual ~TransferSet();

    void add(IElementRef);
    void remove(IElement&);
    void setFocus(ObjectID);

    struct PasteRes
    {
        IElement* focus = nullptr; // ref only, may be zero
        ElementSet pasted_elements;
        PasteRes() {}
    };

    auto paste(Transaction& t) const -> PasteRes;
    // Pastes the contents of the TransferSet into the diagram defined
    // through the active Transaction t.
    // The elements are pasted at the same position from where
    // they were copied.
    // Returns a pointer to the focus element, if there is one in the
    // TransferSet, or zero if there is no focus element.
    // See Inserter::AddFocusCopy below for an explanation of the term
    // "focus element".
    // Note: Paste does NOT change the selection.

    auto startCopy() -> Copier;
    // Deletes the actual TransferSet content and returns an Copier.
    // The client may then insert copies of model elements into the TransferSet
    // through the Copier.
    // There may only be one active Copier per TransferSet at a time.
    // When the Copier is completed(), it will execute all necessary
    // actions which are needed to finalize the copy. For example, the
    // Pointers of the inserted model elements must be corrected to
    // point to the elements in the TransferSet. This can only be done until
    // the complete set of the to-be-copied-elements is known.

    bool isEmpty() const { return elements_.empty(); }

    auto focus() const -> IElement* { return focus_; }

    auto getGroup() -> Group&;

    void assignIDs();

private:
    void clear();
};


class TransferSet::Copier
{
    friend class TransferSet;

    TransferSet& transferSet_;
    std::unique_ptr<CopyRegistry> copyRegistry_; // ownership

public:
    Copier(const Copier&) = delete;
    Copier& operator=(const Copier&) = delete;

    auto addCopy(IElement& m) -> IElement&;
    // Adds a copy of m to the TransferSet which this inserter is bound to.
    // The client must call Complete after the last element has been added.
    // Returns ref to copy

    void addFocusCopy(IElement& focus);
    // Same as AddCopy, but this element is designated as the focus.
    // The TransferSet will be able to say which element is the focus
    // element on a later paste. The focus element is just one specially
    // marked element - nothing more. Focus elements are used by some tasks.

    void complete();
    // Must be called after the last element has been added. This
    // completes a copy operation. After Complete has been called, no more
    // elements may be added with AddCopy or AddFocusCopy.
    // The copier may safely be destructed without calling Complete, but
    // this should only happen in an error case as with an exception.
    // If the copier is destructed without a prior call to Complete,
    // the whole copy operation is aborted and the TransferSet is emptied.
    // Complete may be safely called multiple times.

private:
    Copier(TransferSet& c);
};

}


module : private;

import d1.CastIteratorImp;
import d1.algorithm;


namespace Core
{

TransferSet::~TransferSet()
{
    clear();
}


void TransferSet::add(IElementRef m)
{
    elements_.push_back(m);
    m->setClub(this);
}


void TransferSet::remove(IElement& m)
{
    d1::erase_first_with_get(elements_, m);
}


void TransferSet::setFocus(ObjectID id)
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


void TransferSet::clear()
{
    elements_.clear();
    focus_ = 0;
}


auto TransferSet::paste(Transaction& t) const -> PasteRes
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


void TransferSet::assignIDs()
{
    ObjectID id;

    for (auto mi : elements_)
    {
        auto me = mi.get();
        me->setID(++id);
    }
}


auto TransferSet::getGroup() -> Group&
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


auto TransferSet::startCopy() -> Copier
{
    clear();
    return { *this };
}


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


auto TransferSet::begin() const -> d1::Iterator<IElement*>
{
    return { std::make_unique<IteratorImp>(elements_.begin()) };
}


auto TransferSet::end() const -> d1::Iterator<IElement*>
{
    return { std::make_unique<IteratorImp>(elements_.end()) };
}


TransferSet::Copier::Copier(TransferSet& c):
    transferSet_{ c },
    copyRegistry_{ nullptr }
{
    D1_ASSERT(not transferSet_.copier_);
    transferSet_.copier_ = this;
}


auto TransferSet::Copier::addCopy(IElement& m) -> IElement&
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



void TransferSet::Copier::addFocusCopy(IElement& m)
{
    transferSet_.focus_ = &addCopy(m);
}


void TransferSet::Copier::complete()
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
