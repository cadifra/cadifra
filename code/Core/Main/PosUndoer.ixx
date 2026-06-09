/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:PosUndoer;

import :Undoer;

import std;


namespace Core
{

export class IPosOwner: public virtual IElement
{
public:
    virtual void move(const d1::Vector& offset) = 0;
};


export class PosUndoer: public Undoer
{
    std::shared_ptr<IPosOwner> object_;
    const d1::Vector offset_;

public:
    //-- Undoer

    void undoImp(Param&) override;
    void redoImp(Param&) override;
    bool merge(Undoer& u) override;
    PosUndoer* getPosUndoer() override { return this; }
    void remove(IElement&) override;
    bool isNull() const override;

    //--

    PosUndoer(IPosOwner& theObject, const d1::Vector& offset);

    PosUndoer(const PosUndoer&) = delete;
    PosUndoer& operator=(const PosUndoer&) = delete;

    IPosOwner& object() const { return *object_; }
    const d1::Vector& offset() const { return offset_; }
};

}

namespace Core
{

void PosUndoer::undoImp(Param& p)
{
    if (not object_)
        return;
    object_->move({ -offset_.dx, -offset_.dy });
    p.updateViews({ object_ });
}


void PosUndoer::redoImp(Param& p)
{
    if (not object_)
        return;
    object_->move(offset_);
    p.updateViews({ object_ });
}


bool PosUndoer::merge(Undoer& u)
{
    return false;
}


void PosUndoer::remove(IElement& me)
{
    if (object_.get() == &me)
        object_ = nullptr;
}


bool PosUndoer::isNull() const
{
    return object_.get() == nullptr;
}


PosUndoer::PosUndoer(IPosOwner& po, const d1::Vector& offset):
    object_{},
    offset_{ offset }
{
    auto sp = po.shared_from_this();
    object_ = std::dynamic_pointer_cast<IPosOwner>(sp);
}

}
