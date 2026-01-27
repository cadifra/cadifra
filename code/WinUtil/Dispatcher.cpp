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
    void processImp(Message&) override {}

public:
    NullProcessor():
        IProcessor{ false }
    {
    }
};

NullProcessor* null()
{
    static NullProcessor p;
    return &p;
}

NullProcessor* invalid()
{
    static NullProcessor p;
    return &p;
}


class ObserverHelper
{
    IPrePostDispatchObserver& observer_;
    bool preDispatchCalled_ = false;

public:
    ObserverHelper(IPrePostDispatchObserver& o):
        observer_{ o }
    {
    }

    ~ObserverHelper()
    {
        if (preDispatchCalled_)
            observer_.postDispatchNotification();
    }

    void notify()
    {
        if (not preDispatchCalled_)
        {
            observer_.preDispatchNotification();
            preDispatchCalled_ = true;
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
    void add(ProcRegistrar*) final;
    void forget(ProcRegistrar*) final;

    void newProcessor(unsigned int msgId) final;
    void newSpy(unsigned int msgId, const std::weak_ptr<ISpy>&) final;

    void dispatch(Message&, IPrePostDispatchObserver&);

private:
    using ProcessorMap = std::map<unsigned int, IProcessor*>;
    using RegistrarVector = std::vector<ProcRegistrar*>;

    ProcessorMap processorMap_;
    RegistrarVector registrars_;

    using SpyMap = std::map<unsigned int, std::list<std::weak_ptr<ISpy>>>;

    SpyMap spyMap_;

    void invalidatePlugMap();
    void dispatchSpy(Message&, ObserverHelper&);
};


void C::Impl::invalidatePlugMap()
{
    for (auto& e : processorMap_)
        e.second = invalid();
}


void C::Impl::add(ProcRegistrar* r)
{
    D1_ASSERT(std::ranges::find(registrars_, r) == end(registrars_));

    registrars_.push_back(r);

    invalidatePlugMap();
}


void C::Impl::forget(ProcRegistrar* r)
{
    auto i = std::ranges::find(registrars_, r);

    D1_ASSERT(i != end(registrars_));

    registrars_.erase(i);

    if (registrars_.empty())
        processorMap_.clear();
    else
        invalidatePlugMap();
}


void C::Impl::newProcessor(unsigned int msgId)
{
    processorMap_[msgId] = invalid();
}


void C::Impl::newSpy(unsigned int msgId, const std::weak_ptr<ISpy>& weak)
{
    spyMap_[msgId].push_front(weak);
}


void C::Impl::dispatchSpy(Message& m, ObserverHelper& oh)
{
    auto i = spyMap_.find(m.getMsgId());
    if (i == end(spyMap_))
        return;

    auto& list = i->second;

    oh.notify();

    for (const auto& weak : list)
        if (not weak.expired())
            weak.lock()->spy(m); // may add or delete spy plugs

    // Don't care about empty lists or lists with many expired pointers:
    // 1. it doesn't hurt
    // 2. it doesn't happen
}


void C::Impl::dispatch(Message& m, IPrePostDispatchObserver& ppdo)
{
    auto delay_deletion = shared_from_this();

    auto oh = ObserverHelper{ ppdo };
    dispatchSpy(m, oh);

    auto i = processorMap_.find(m.getMsgId());

    if (i == end(processorMap_))
    {
        m.enableOS();
        return;
    }

    if (i->second == invalid())
    {
        D1_ASSERT(not registrars_.empty());

        auto r = rbegin(registrars_);

        IProcessor* p = (*r)->getProcessor(m.getMsgId());

        if (p)
            i->second = p;
        else
        {
            i->second = null();

            for (; (r != rend(registrars_)) and not p; ++r)
                p = (*r)->getProcessor(m.getMsgId());

            if (not p)
            {
                processorMap_.erase(i);
                m.enableOS();
                return;
            }

            if (p->isAlwaysReady())
                i->second = p;
        }
    }

    oh.notify();
    i->second->process(m);
}


C::Dispatcher():
    impl_{ std::make_shared<Impl>() }
{
}


C::~Dispatcher() = default;


C::operator IDispatcher&() const
{
    return *impl_;
}

void C::dispatch(Message& msg, IPrePostDispatchObserver& pp) const
{
    impl_->dispatch(msg, pp);
}

}
