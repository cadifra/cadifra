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

    ProcessorMap itsProcessorMap;
    IDispatcher& itsDispatcher;
    ProcRegistrar* itsRegistrar = nullptr;
    std::vector<std::shared_ptr<ISpy>> itsSpys;
    IExceptionHandler* const itsExceptionHandler = nullptr; // may be zero, no ownership

public:
    Impl(IDispatcher&, ProcRegistrar*, IExceptionHandler*);

    ~Impl(); // intentionally not virtual

    void Register(unsigned int msgId, std::unique_ptr<IProcessor> p);
    IProcessor* GetProcessor(unsigned int msgId) const;

    void RegisterSpy(unsigned int msgId, const std::shared_ptr<ISpy>&);
};


C::Impl::Impl(
    IDispatcher& d,
    ProcRegistrar* r,
    IExceptionHandler* eh):

    itsDispatcher{ d },
    itsRegistrar{ r },
    itsExceptionHandler{ eh }
{
    itsDispatcher.Register(r);
}


C::Impl::~Impl()
{
    itsDispatcher.Unregister(itsRegistrar);
}


void C::Impl::Register(unsigned int msgId, std::unique_ptr<IProcessor> p)
{
    auto i = itsProcessorMap.find(msgId);
    if (i != end(itsProcessorMap))
    {
        D1_ASSERT(0); // id already registered
        return;
    }

    p->SetExceptionHandler(itsExceptionHandler);

    itsProcessorMap[msgId] = std::move(p);

    itsDispatcher.NewProcessor(msgId);
}


IProcessor* C::Impl::GetProcessor(unsigned int msgId) const
{
    if (auto i = itsProcessorMap.find(msgId); i != end(itsProcessorMap))
        return i->second.get();

    return nullptr;
}


void C::Impl::RegisterSpy(unsigned int msgId, const std::shared_ptr<ISpy>& s)
{
    D1_ASSERT(s);

    itsSpys.push_back(s);

    itsDispatcher.NewSpy(msgId, s);
}


C::ProcRegistrar(IDispatcher& d, IExceptionHandler* eh):
    itsImpl{ std::make_unique<Impl>(d, this, eh) }
{
}


C::~ProcRegistrar() = default;


void C::Register(unsigned int msgId, std::unique_ptr<IProcessor> p)
{
    itsImpl->Register(msgId, std::move(p));
}


IProcessor* C::GetProcessor(unsigned int msgId) const
{
    return itsImpl->GetProcessor(msgId);
}


void C::RegisterSpy(unsigned int msgId, const std::shared_ptr<ISpy>& s)
{
    itsImpl->RegisterSpy(msgId, s);
}


void IProcessor::Process(Message& msg)
{
    if (!itsExceptionHandler)
    {
        ProcessImp(msg);
        return;
    }

    D1_ASSERT(itsExceptionHandler);

    try
    {
        ProcessImp(msg);
    }
    catch (...)
    {
        itsExceptionHandler->HandleException();
        throw;
    }
}

}
