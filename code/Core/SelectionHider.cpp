/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

namespace
{
using C = Selection::Hider;
}


C::~Hider()
{
    if (itsServerImp)
        itsServerImp->RemoveHider();
}


C::Hider(const Hider& rhs):
    itsServerImp{ rhs.itsServerImp }
{
    if (itsServerImp)
        itsServerImp->AddHider();
}


auto C::operator=(const Hider& rhs) -> Hider&
{
    if (rhs.itsServerImp)
        rhs.itsServerImp->AddHider();
    if (itsServerImp)
        itsServerImp->RemoveHider();
    itsServerImp = rhs.itsServerImp;
    return *this;
}


C::Hider(const std::shared_ptr<VS::Imp>& server):

    itsServerImp{ server }
{
    if (itsServerImp)
        itsServerImp->AddHider();
}

}
