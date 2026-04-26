/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core.Main;


namespace Core
{

namespace
{
using C = PosUndoer;
}


void C::undoImp(Param& p)
{
    if (not object_)
        return;
    object_->move({ -offset_.dx, -offset_.dy });
    p.updateViews({ object_ });
}


void C::redoImp(Param& p)
{
    if (not object_)
        return;
    object_->move(offset_);
    p.updateViews({ object_ });
}


bool C::merge(Undoer& u)
{
    return false;
}


void C::remove(IElement& me)
{
    if (object_.get() == &me)
        object_ = nullptr;
}


bool C::isNull() const
{
    return object_.get() == nullptr;
}


C::PosUndoer(IPosOwner& po, const d1::Vector& offset):
    object_{},
    offset_{ offset }
{
    auto sp = po.shared_from_this();
    object_ = std::dynamic_pointer_cast<IPosOwner>(sp);
}

}
