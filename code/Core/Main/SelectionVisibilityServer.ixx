/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.Main:SelectionVisibilityServer;

import std;

namespace Core
{

export {
class IView;
}

namespace Selection
{

export class Hider;

export class VisibilityServer
{
public:
    VisibilityServer(IView& v):
        view_{ v }
    {
    }

    VisibilityServer(const VisibilityServer& rhs) = delete;
    VisibilityServer& operator=(const VisibilityServer& rhs) = delete;

    ~VisibilityServer();

    Hider hideSelection();
    // Makes the selection invisible in view_. If the returned Hider
    // is destructed, the selection is made visible again (if there are no
    // other selection hiders at this VisibilityServer)

    class Imp;

private:
    std::shared_ptr<Imp> imp_;
    IView& view_;
};

class VisibilityServer::Imp final
{
    friend class VisibilityServer;
    friend class Hider;

    unsigned long numOfServers_{};
    IView* view_{};
    unsigned long numOfHiders_{};

    void addServer(IView& v);
    void releaseServer();

    void addHider();
    void removeHider();
};


export class Hider // has value semantics
{
    using VS = VisibilityServer;

    friend class VS;

    std::shared_ptr<VS::Imp> serverImp_; // may be zero

public:
    Hider() = default;
    ~Hider()
    {
        if (serverImp_)
            serverImp_->removeHider();
    }

    Hider(const Hider& rhs):
        serverImp_{ rhs.serverImp_ }
    {
        if (serverImp_)
            serverImp_->addHider();
    }

    Hider& operator=(const Hider& rhs)
    {
        if (rhs.serverImp_)
            rhs.serverImp_->addHider();
        if (serverImp_)
            serverImp_->removeHider();
        serverImp_ = rhs.serverImp_;
        return *this;
    }

private:
    Hider(const std::shared_ptr<VS::Imp>& server):
        serverImp_{ server }
    {
        if (serverImp_)
            serverImp_->addHider();
    }
};

}

}

import :IView;

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
