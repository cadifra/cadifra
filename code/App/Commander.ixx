/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module App.Commander;

import App.Command;

import WinUtil.IWindow;

import std;


namespace App
{

export class Commander
{
public:
    Commander(const WinUtil::IWindow&);

    Commander(const Commander&) = delete;
    Commander& operator=(const Commander&) = delete;

    ~Commander(); // intentionally not virtual

    auto get() const -> ICommander&;

private:
    class Impl;
    std::shared_ptr<Impl> impl_;
};

}


module : private;

import App.ICmdView;
import App.CmdState;

import d1.algorithm;

import WinUtil.Messages;
import WinUtil.ProcRegistrar;

import std;


namespace App
{

class Cmd
{
public:
    void connect(ICommand&); // reference only
    void disconnect(ICommand&);
    void enableNotification(ICommand&);


    void add(const std::shared_ptr<ICmdView>&);
    void Delete(ICmdView&);

    void send(const WinUtil::WM_COMMAND_Msg&);
    void setState(CmdState);

private:
    void computeEnableState();
    void notify(ICmdView&) const;
    void removeNulls();

    using ItsViews = std::vector<std::shared_ptr<ICmdView>>;
    using ItsPlugs = std::vector<ICommand*>;

    ItsViews views_;
    ItsPlugs plugs_; // ref only pointers
    bool enabled_ = false;
    CmdState state_;

    int inSend_ = 0;
    bool hasNulls_ = false;

    friend class IncDecIsInSend;
};


class IncDecIsInSend
{
    Cmd& cmd_;

public:
    IncDecIsInSend(Cmd& cmd):
        cmd_{ cmd }
    {
        ++cmd_.inSend_;
    }
    ~IncDecIsInSend()
    {
        --cmd_.inSend_;
    }
};


void Cmd::connect(ICommand& p)
{
    D1_ASSERT(not inSend_);

    D1_ASSERT(end(plugs_) == std::ranges::find(plugs_, &p));

    plugs_.push_back(&p);

    computeEnableState();
}


void Cmd::disconnect(ICommand& p)
{
    auto i = std::ranges::find(plugs_, &p);

    D1_ASSERT(i != end(plugs_));

    *i = 0;
    hasNulls_ = true;

    computeEnableState();
}


void Cmd::enableNotification(ICommand& p)
{
    auto i = std::ranges::find(plugs_, &p);

    D1_ASSERT(i != end(plugs_));

    computeEnableState();
}


void Cmd::add(const std::shared_ptr<ICmdView>& v)
{
    D1_ASSERT(end(views_) == std::ranges::find(views_, v));

    views_.push_back(v);
    notify(*v);
}


void Cmd::Delete(ICmdView& v)
{
    d1::erase_first_with_get(views_, v);
}


void Cmd::send(const WinUtil::WM_COMMAND_Msg& msg)
{
    auto t = IncDecIsInSend{ *this };

    auto enabled = [](auto p) { return p and p->isEnabled(); };

    for (auto* p : plugs_ | std::views::filter(enabled))
        if (not p->prepare(msg))
            return;

    for (auto* p : plugs_ | std::views::filter(enabled))
        p->execute(msg);

    removeNulls();
}


void Cmd::computeEnableState()
{
    bool wasEnabled = enabled_;

    enabled_ = false;

    auto enabled = [](auto p) { return p and p->isEnabled(); };

    for (auto* p : plugs_ | std::views::filter(enabled))
    {
        enabled_ = true;
        break;
    }

    if (wasEnabled != enabled_)
        for (const auto& cv : views_)
            notify(*cv);
}


void Cmd::notify(ICmdView& v) const
{
    v.update(enabled_, state_);
}


void Cmd::removeNulls()
{
    if (not hasNulls_)
        return;

    std::erase(plugs_, nullptr);

    hasNulls_ = false;
}


void Cmd::setState(CmdState s)
{
    bool do_notify = s != state_;
    state_ = s;

    if (do_notify)
        for (const auto& cv : views_)
            notify(*cv);
}


using C = Commander;

class C::Impl:
    public ICommander
{
public:
    Impl(std::shared_ptr<Impl>& ptr, const WinUtil::IWindow& w);

    void hire(ICommand&) final;
    void fire(ICommand&) final;
    void enableNotification(ICommand&) final;

    void add(const std::shared_ptr<ICmdView>&) final;
    void forget(ICmdView&) final;

    auto getOwner() -> const WinUtil::IWindow& final { return window_; }

    void setState(int id, CmdState) final;

    void onCommand(WinUtil::WM_COMMAND_Msg);

private:
    using ItsCmds = std::map<int, Cmd>;
    ItsCmds cmds_;

    const WinUtil::IWindow& window_;
    WinUtil::ProcRegistrar registrar_;

    std::shared_ptr<Impl>& sharedPtr_;
};


C::Impl::Impl(std::shared_ptr<Impl>& ptr, const WinUtil::IWindow& w):
    sharedPtr_{ ptr },
    registrar_{ w.getDispatcher(), 0 }, window_{ w }
{
    auto r = registrar_.helper(*this);

    r.addAlwaysReady(&Impl::onCommand);
}


void C::Impl::enableNotification(ICommand& p)
{
    auto i = cmds_.find(p.getId());

    D1_ASSERT(i != end(cmds_));

    i->second.enableNotification(p);
}


void C::Impl::hire(ICommand& p)
{
    cmds_[p.getId()].connect(p);
}


void C::Impl::fire(ICommand& p)
{
    auto i = cmds_.find(p.getId());

    D1_ASSERT(i != end(cmds_));

    i->second.disconnect(p);
}


void C::Impl::add(const std::shared_ptr<ICmdView>& v)
{
    auto id = v->getId();
    cmds_[id].add(v);
}


void C::Impl::forget(ICmdView& v)
{
    auto i = cmds_.find(v.getId());

    D1_ASSERT(i != end(cmds_));

    i->second.Delete(v);
}


void C::Impl::onCommand(WinUtil::WM_COMMAND_Msg m)
{
    auto protect = sharedPtr_; // increase usage count of sharedPtr_

    auto i = cmds_.find(m.wID());

    if (i != end(cmds_))
        i->second.send(m);

    m.enableOS();
}


void C::Impl::setState(int id, CmdState s)
{
    auto i = cmds_.find(id);
    D1_ASSERT(i != end(cmds_));
    i->second.setState(s);
}


C::Commander(const WinUtil::IWindow& w):
    impl_{ std::make_shared<Impl>(impl_, w) }
{
}


C::~Commander()
{
}


auto C::get() const -> ICommander&
{
    return *impl_;
}

}
