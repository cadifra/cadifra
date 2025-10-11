/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

namespace
{
using C = SelectionHider;
}


C::~SelectionHider()
{
    if (itsServerImp)
        itsServerImp->RemoveHider();
}


C::SelectionHider(const SelectionHider& rhs):
    itsServerImp{ rhs.itsServerImp }
{
    if (itsServerImp)
        itsServerImp->AddHider();
}


auto C::operator=(const SelectionHider& rhs) -> SelectionHider&
{
    if (rhs.itsServerImp)
        rhs.itsServerImp->AddHider();
    if (itsServerImp)
        itsServerImp->RemoveHider();
    itsServerImp = rhs.itsServerImp;
    return *this;
}


C::SelectionHider(const std::shared_ptr<SelectionVisibilityServerImp>& server):

    itsServerImp{ server }
{
    if (itsServerImp)
        itsServerImp->AddHider();
}

}
