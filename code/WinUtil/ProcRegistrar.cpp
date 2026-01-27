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
using C = ProcRegistrar;
}


class C::Impl
{
    using ProcessorMap = std::map<unsigned int, std::unique_ptr<IProcessor>>;

    ProcessorMap processorMap_;
    IDispatcher& dispatcher_;
    ProcRegistrar* registrar_ = nullptr;
    std::vector<std::shared_ptr<ISpy>> spys_;
    IExceptionHandler* const exceptionHandler_ = nullptr; // may be zero, no ownership

public:
    Impl(IDispatcher&, ProcRegistrar*, IExceptionHandler*);

    ~Impl(); // intentionally not virtual

    void add(unsigned int msgId, std::unique_ptr<IProcessor> p);
    IProcessor* getProcessor(unsigned int msgId) const;

    void add(unsigned int msgId, const std::shared_ptr<ISpy>&);
};


C::Impl::Impl(
    IDispatcher& d,
    ProcRegistrar* r,
    IExceptionHandler* eh):

    dispatcher_{ d },
    registrar_{ r },
    exceptionHandler_{ eh }
{
    dispatcher_.add(r);
}


C::Impl::~Impl()
{
    dispatcher_.forget(registrar_);
}


void C::Impl::add(unsigned int msgId, std::unique_ptr<IProcessor> p)
{
    auto i = processorMap_.find(msgId);
    if (i != end(processorMap_))
    {
        D1_ASSERT(0); // id already registered
        return;
    }

    p->setExceptionHandler(exceptionHandler_);

    processorMap_[msgId] = std::move(p);

    dispatcher_.newProcessor(msgId);
}


IProcessor* C::Impl::getProcessor(unsigned int msgId) const
{
    if (auto i = processorMap_.find(msgId); i != end(processorMap_))
        return i->second.get();

    return nullptr;
}


void C::Impl::add(unsigned int msgId, const std::shared_ptr<ISpy>& s)
{
    D1_ASSERT(s);

    spys_.push_back(s);

    dispatcher_.newSpy(msgId, s);
}


C::ProcRegistrar(IDispatcher& d, IExceptionHandler* eh):
    impl_{ std::make_unique<Impl>(d, this, eh) }
{
}


C::~ProcRegistrar() = default;


void C::add(unsigned int msgId, std::unique_ptr<IProcessor> p)
{
    impl_->add(msgId, std::move(p));
}


IProcessor* C::getProcessor(unsigned int msgId) const
{
    return impl_->getProcessor(msgId);
}


void C::add(unsigned int msgId, const std::shared_ptr<ISpy>& s)
{
    impl_->add(msgId, s);
}


void IProcessor::process(Message& msg)
{
    if (not exceptionHandler_)
    {
        processImp(msg);
        return;
    }

    D1_ASSERT(exceptionHandler_);

    try
    {
        processImp(msg);
    }
    catch (...)
    {
        exceptionHandler_->handleException();
        throw;
    }
}

}
