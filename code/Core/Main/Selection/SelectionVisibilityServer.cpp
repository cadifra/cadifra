/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core.Main;


namespace Core
{

namespace
{
using C = Selection::VisibilityServer;
}


void C::Imp::addServer(IView& v)
{
    if (not view_)
        view_ = &v;
    D1_ASSERT(view_ == &v);
    ++numOfServers_;
}


void C::Imp::releaseServer()
{
    if (--numOfServers_ == 0)
        view_ = nullptr;
}


void C::Imp::addHider()
{
    if (numOfHiders_++ == 0)
        if (numOfServers_ and view_)
            view_->setSelectionVisibility(false);
}


void C::Imp::removeHider()
{
    if (--numOfHiders_ == 0)
        if (numOfServers_)
            view_->setSelectionVisibility(true);
}


C::~VisibilityServer()
{
    if (imp_)
        imp_->releaseServer();
}


auto C::hideSelection() -> Hider
{
    if (not imp_)
    {
        imp_ = std::make_shared<VisibilityServer::Imp>();
        imp_->addServer(view_);
    }
    return { imp_ };
}

}
