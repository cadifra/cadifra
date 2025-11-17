/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module WinUtil.ProcRegistrar;

import std;


namespace WinUtil
{

namespace
{

class NullProcessor: public IProcessor
{
    void ProcessImp(Message&) override {}

public:
    NullProcessor():
        IProcessor{ false }
    {
    }
};

NullProcessor* Null()
{
    static NullProcessor p;
    return &p;
}

NullProcessor* Invalid()
{
    static NullProcessor p;
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

using C = Dispatcher;
}


class C::Impl:
    public IDispatcher,
    public std::enable_shared_from_this<Impl>
{
public:
    void Register(ProcRegistrar*) final;
    void Unregister(ProcRegistrar*) final;

    void NewProcessor(unsigned int msgId) final;
    void NewSpy(unsigned int msgId, const std::weak_ptr<ISpy>&) final;

    void Dispatch(Message&, IPrePostDispatchObserver&);

private:
    using ProcessorMap = std::map<unsigned int, IProcessor*>;
    using RegistrarVector = std::vector<ProcRegistrar*>;

    ProcessorMap itsProcessorMap;
    RegistrarVector itsRegistrars;

    using SpyMap = std::map<unsigned int, std::list<std::weak_ptr<ISpy>>>;

    SpyMap itsSpyMap;

    void InvalidatePlugMap();
    void DispatchSpy(Message&, ObserverHelper&);
};


void C::Impl::InvalidatePlugMap()
{
    for (auto& e : itsProcessorMap)
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
        itsProcessorMap.clear();
    else
        InvalidatePlugMap();
}


void C::Impl::NewProcessor(unsigned int msgId)
{
    itsProcessorMap[msgId] = Invalid();
}


void C::Impl::NewSpy(unsigned int msgId, const std::weak_ptr<ISpy>& weak)
{
    itsSpyMap[msgId].push_front(weak);
}


void C::Impl::DispatchSpy(Message& m, ObserverHelper& oh)
{
    auto i = itsSpyMap.find(m.GetMsgId());
    if (i == end(itsSpyMap))
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


void C::Impl::Dispatch(Message& m, IPrePostDispatchObserver& ppdo)
{
    auto delay_deletion = shared_from_this();

    auto oh = ObserverHelper{ ppdo };
    DispatchSpy(m, oh);

    auto i = itsProcessorMap.find(m.GetMsgId());

    if (i == end(itsProcessorMap))
    {
        m.EnableOS();
        return;
    }

    if (i->second == Invalid())
    {
        D1_ASSERT(!itsRegistrars.empty());

        auto r = rbegin(itsRegistrars);

        IProcessor* p = (*r)->GetProcessor(m.GetMsgId());

        if (p)
            i->second = p;
        else
        {
            i->second = Null();

            for (; (r != rend(itsRegistrars)) && !p; ++r)
                p = (*r)->GetProcessor(m.GetMsgId());

            if (!p)
            {
                itsProcessorMap.erase(i);
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


C::Dispatcher():
    itsImpl{ std::make_shared<Impl>() }
{
}


C::~Dispatcher() = default;


C::operator IDispatcher&() const
{
    return *itsImpl;
}

void C::Dispatch(Message& msg, IPrePostDispatchObserver& pp) const
{
    itsImpl->Dispatch(msg, pp);
}

}
