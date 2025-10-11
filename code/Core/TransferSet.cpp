/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;

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
    Clear();
}


void C::Add(IElementPtr m)
{
    itsElements.push_back(m);
    m->SetClub(this);
}


void C::Remove(IElement& m)
{
    d1::erase_first_with_get(itsElements, m);
}


void C::SetFocus(ObjectID id)
{
    itsFocus = 0;

    for (auto mi : itsElements)
    {
        auto& me = *mi.get();
        if (me.GetID() == id)
        {
            itsFocus = &me;
            return;
        }
    }
}


void C::Clear()
{
    itsElements.clear();
    itsFocus = 0;
}


auto C::Paste(Transaction& t) const -> PasteRes
{
    PasteRes res;

    D1_ASSERT(!itsCopier); // paste is not allowed while there is an active copier

    if (itsElements.empty())
        return res; // nothing to paste

    auto r = CopyRegistry::MakeNew();

    MeSet copies;

    // first pass: create all copies --------------------------------------------

    for (auto mi : itsElements)
    {
        const auto& orig = *mi.get();

        if (!orig.AcceptForPaste(t.Diagram()))
            continue;

        auto copy{ orig.Copy() };

        copies.push_back(copy);
        r->Register(&orig, copy.get());

        if (&orig == itsFocus)
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
        copy->AdjustRefsAfterCopy(*r);
    }

    // ## copies are consistent now and may be used
    for (auto mi : copies)
        t.AddNewlyCreated(mi);

    for (auto mi : copies)
        res.pasted_elements.Insert(*mi.get());

    return res;
}


void C::AssignIDs()
{
    ObjectID id;

    for (auto mi : itsElements)
    {
        auto me = mi.get();
        me->SetID(++id);
    }
}


auto C::GetGroup() -> Group&
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

        void AssignIDs() final { ts_.AssignIDs(); };

        SI begin() const final
        {
            return { std::make_unique<CastIter>(ts_.begin()) };
        }

        SI end() const final
        {
            return { std::make_unique<CastIter>(ts_.end()) };
        }
    };

    if (!itsGroup)
        itsGroup = std::make_unique<GroupImp>(*this);

    return *itsGroup.get();
}


auto C::StartCopy() -> Copier
{
    Clear();
    return { *this };
}


namespace
{
class IteratorImp: public d1::IIteratorImp<IElement*>
{
    using C = TransferSet::MeSet;

    C::const_iterator itsIterator;

    auto Clone() const -> std::unique_ptr<IIteratorImp> final
    {
        return std::make_unique<IteratorImp>(itsIterator);
    }

    bool IsEqual(const IIteratorImp& rhs) const final
    {
        return itsIterator == static_cast<const IteratorImp&>(rhs).itsIterator;
    }

    void Next() final { ++itsIterator; }

    value_type Val() const final { return itsIterator->get(); }

public:
    IteratorImp(C::const_iterator i):
        itsIterator{ i }
    {
    }
};
}


auto C::begin() const -> d1::Iterator<IElement*>
{
    return { std::make_unique<IteratorImp>(itsElements.begin()) };
}


auto C::end() const -> d1::Iterator<IElement*>
{
    return { std::make_unique<IteratorImp>(itsElements.end()) };
}


C::Copier::Copier(TransferSet& c):
    itsTransferSet{ c },
    itsCopyRegistry{ nullptr }
{
    D1_ASSERT(!itsTransferSet.itsCopier);
    itsTransferSet.itsCopier = this;
}


auto C::Copier::AddCopy(IElement& m) -> IElement&
{
    if (!itsCopyRegistry)
        itsCopyRegistry = CopyRegistry::MakeNew();

    D1_ASSERT(itsCopyRegistry);
    D1_ASSERT(itsTransferSet.itsCopier == this);

    auto copy{ m.Copy() };
    itsTransferSet.Add(copy);

    itsCopyRegistry->Register(/*original*/ &m, /*copy*/ copy.get());

    return *copy.get();
}



void C::Copier::AddFocusCopy(IElement& m)
{
    itsTransferSet.itsFocus = &AddCopy(m);
}


void C::Copier::Complete()
{
    if (!itsCopyRegistry)
        return;

    D1_ASSERT(itsCopyRegistry);
    D1_ASSERT(itsTransferSet.itsCopier == this);

    for (auto mi : itsTransferSet.itsElements)
    {
        auto& me = *mi.get();
        me.AdjustRefsAfterCopy(*itsCopyRegistry);
    }

    itsTransferSet.itsCopier = 0;

    itsCopyRegistry.reset();
}

}
