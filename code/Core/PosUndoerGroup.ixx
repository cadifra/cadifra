/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.PosUndoerGroup;

import Core.Main;

import d1.Point;


namespace Core
{

export class PosUndoerGroup: public Undoer
{
    using V = std::vector<std::shared_ptr<IPosOwner>>;

    V objects_;
    const d1::Vector offset_;

public:
    //-- Undoer

    void undoImp(Param&) override;
    void redoImp(Param&) override;
    bool merge(Undoer& u) override { return false; }
    void remove(IElement&) override;
    bool isNull() const override { return objects_.empty(); }

    //--

    PosUndoerGroup(const d1::Vector& offset): offset_{ offset } {}

    PosUndoerGroup(const PosUndoerGroup&) = delete;
    PosUndoerGroup& operator=(const PosUndoerGroup&) = delete;

    void add(IPosOwner&);

    const d1::Vector& offset() const { return offset_; }
};

}

module :private;

import d1.algorithm;


namespace Core
{

void PosUndoerGroup::undoImp(Param& p)
{
    const auto delta = d1::Vector{ -offset_.dx, -offset_.dy };

    for (auto o : objects_)
    {
        o->move(delta);
        p.updateViews({ o });
    }
}


void PosUndoerGroup::redoImp(Param& p)
{
    for (auto o : objects_)
    {
        o->move(offset_);
        p.updateViews({ o });
    }
}


void PosUndoerGroup::remove(IElement& me)
{
    d1::erase_first_with_get(objects_, me);
}


void PosUndoerGroup::add(IPosOwner& po)
{
    D1_ASSERT(&po);

    auto sp = po.shared_from_this();
    auto spo = std::dynamic_pointer_cast<IPosOwner>(sp);

    objects_.push_back(spo);
}

}
