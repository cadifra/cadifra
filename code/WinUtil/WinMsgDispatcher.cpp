/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module WinUtil;

import std;


namespace WinUtil
{

namespace
{

class NullPlug: public IWinMsgPlug
{
    void ProcessImp(WinMsg&) override {}

public:
    NullPlug():
        IWinMsgPlug{ false }
    {
    }
};

NullPlug* Null()
{
    static NullPlug p;
    return &p;
}

NullPlug* Invalid()
{
    static NullPlug p;
    return &p;
}


class ObserverHelper
{
    IPrePostDispatchObserver& itsObserver;
    bool itsPreDispatchCalled = false;

public:
    ObserverHelper(IPrePostDispatchObserver& o):
        itsObserver{ o }
    {
    }

    ~ObserverHelper()
    {
        if (itsPreDispatchCalled)
            itsObserver.PostDispatchNotification();
    }

    void Notify()
    {
        if (!itsPreDispatchCalled)
        {
            itsObserver.PreDispatchNotification();
            itsPreDispatchCalled = true;
        }
    }
};

using C = WinMsgDispatcher;
}


class C::Impl:
    public IWinMsgDispatcher,
    public std::enable_shared_from_this<Impl>
{
public:
    void Register(ProcRegistrar*) final;
    void Unregister(ProcRegistrar*) final;
    ;
    void NewPlug(unsigned int msgId) final;
    void NewSpyPlug(unsigned int msgId,
        const std::weak_ptr<ISpyWinMsgPlug>&) final;

    void Dispatch(WinMsg&, IPrePostDispatchObserver&);

private:
    using PlugMap = std::map<unsigned int, IWinMsgPlug*>;
    using RegistrarVector = std::vector<ProcRegistrar*>;

    PlugMap itsPlugMap;
    RegistrarVector itsRegistrars;

    using SpyPlugMap = std::map<unsigned int,
        std::list<std::weak_ptr<ISpyWinMsgPlug>>>;

    SpyPlugMap itsSpyPlugMap;

    void InvalidatePlugMap();
    void DispatchSpy(WinMsg&, ObserverHelper&);
};


void C::Impl::InvalidatePlugMap()
{
    for (auto& e : itsPlugMap)
        e.second = Invalid();
}


void C::Impl::Register(ProcRegistrar* r)
{
    D1_ASSERT(std::ranges::find(itsRegistrars, r) == end(itsRegistrars));

    itsRegistrars.push_back(r);

    InvalidatePlugMap();
}


void C::Impl::Unregister(ProcRegistrar* r)
{
    auto i = std::ranges::find(itsRegistrars, r);

    D1_ASSERT(i != end(itsRegistrars));

    itsRegistrars.erase(i);

    if (itsRegistrars.empty())
        itsPlugMap.clear();
    else
        InvalidatePlugMap();
}


void C::Impl::NewPlug(unsigned int msgId)
{
    itsPlugMap[msgId] = Invalid();
}


void C::Impl::NewSpyPlug(
    unsigned int msgId, const std::weak_ptr<ISpyWinMsgPlug>& weak)
{
    itsSpyPlugMap[msgId].push_front(weak);
}


void C::Impl::DispatchSpy(WinMsg& m, ObserverHelper& oh)
{
    auto i = itsSpyPlugMap.find(m.GetMsgId());
    if (i == end(itsSpyPlugMap))
        return;

    auto& list = i->second;

    oh.Notify();

    for (const auto& weak : list)
        if (!weak.expired())
            weak.lock()->Spy(m); // may add or delete spy plugs

    // Don't care about empty lists or lists with many expired pointers:
    // 1. it doesn't hurt
    // 2. it doesn't happen
}


void C::Impl::Dispatch(WinMsg& m, IPrePostDispatchObserver& ppdo)
{
    auto delay_deletion = shared_from_this();

    auto oh = ObserverHelper{ ppdo };
    DispatchSpy(m, oh);

    auto i = itsPlugMap.find(m.GetMsgId());

    if (i == end(itsPlugMap))
    {
        m.EnableOS();
        return;
    }

    if (i->second == Invalid())
    {
        D1_ASSERT(!itsRegistrars.empty());

        auto r = rbegin(itsRegistrars);

        IWinMsgPlug* p = (*r)->GetWinMsgPlug(m.GetMsgId());

        if (p)
            i->second = p;
        else
        {
            i->second = Null();

            for (; (r != rend(itsRegistrars)) && !p; ++r)
                p = (*r)->GetWinMsgPlug(m.GetMsgId());

            if (!p)
            {
                itsPlugMap.erase(i);
                m.EnableOS();
                return;
            }

            if (p->IsAlwaysReady())
                i->second = p;
        }
    }

    oh.Notify();
    i->second->Process(m);
}


C::WinMsgDispatcher():
    itsImpl{ std::make_shared<Impl>() }
{
}


C::~WinMsgDispatcher() = default;


C::operator IWinMsgDispatcher&() const
{
    return *itsImpl;
}

void C::Dispatch(WinMsg& msg, IPrePostDispatchObserver& pp) const
{
    itsImpl->Dispatch(msg, pp);
}

}
