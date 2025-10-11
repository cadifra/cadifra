/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

namespace
{
using C = PosUndoer;
}


void C::UndoImp(UndoerParam& p)
{
    if (!itsObject)
        return;
    itsObject->Move({ -itsOffset.dx, -itsOffset.dy });
    p.UpdateViews({ itsObject });
}


void C::RedoImp(UndoerParam& p)
{
    if (!itsObject)
        return;
    itsObject->Move(itsOffset);
    p.UpdateViews({ itsObject });
}


bool C::Merge(Undoer& u)
{
    return false;
}


void C::Remove(IElement& me)
{
    if (itsObject.get() == &me)
        itsObject = nullptr;
}


bool C::IsNull() const
{
    return itsObject.get() == nullptr;
}


C::PosUndoer(IPosOwner& po, const d1::Vector& offset):
    itsObject{},
    itsOffset{ offset }
{
    auto sp = po.shared_from_this();
    itsObject = std::dynamic_pointer_cast<IPosOwner>(sp);
}

}
