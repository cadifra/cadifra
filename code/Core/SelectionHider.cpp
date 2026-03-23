/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core:Base;


namespace Core
{

namespace
{
using C = Selection::Hider;
}


C::~Hider()
{
    if (serverImp_)
        serverImp_->removeHider();
}


C::Hider(const Hider& rhs):
    serverImp_{ rhs.serverImp_ }
{
    if (serverImp_)
        serverImp_->addHider();
}


auto C::operator=(const Hider& rhs) -> Hider&
{
    if (rhs.serverImp_)
        rhs.serverImp_->addHider();
    if (serverImp_)
        serverImp_->removeHider();
    serverImp_ = rhs.serverImp_;
    return *this;
}


C::Hider(const std::shared_ptr<VS::Imp>& server):

    serverImp_{ server }
{
    if (serverImp_)
        serverImp_->addHider();
}

}
