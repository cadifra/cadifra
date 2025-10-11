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
using C = ProcRegistrar;
}


class C::Impl
{
    using PlugMap =
        std::map<
            unsigned int,
            std::unique_ptr<IWinMsgPlug>>;

    PlugMap itsPlugMap;
    IWinMsgDispatcher& itsDispatcher;
    ProcRegistrar* itsRegistrar = nullptr;
    std::vector<std::shared_ptr<ISpyWinMsgPlug>> itsSpyPlugs;
    IExceptionHandler* const itsExceptionHandler = nullptr; // may be zero, no ownership

public:
    Impl(
        IWinMsgDispatcher&, ProcRegistrar*, IExceptionHandler*);

    ~Impl(); // intentionally not virtual

    void Register(unsigned int msgId, std::unique_ptr<IWinMsgPlug> p);
    IWinMsgPlug* GetWinMsgPlug(unsigned int msgId) const;

    void RegisterSpy(unsigned int msgId, const std::shared_ptr<ISpyWinMsgPlug>&);
};


C::Impl::Impl(
    IWinMsgDispatcher& d,
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


void C::Impl::Register(
    unsigned int msgId, std::unique_ptr<IWinMsgPlug> p)
{
    auto i = itsPlugMap.find(msgId);
    if (i != end(itsPlugMap))
    {
        D1_ASSERT(0); // id already registered
        return;
    }

    p->SetExceptionHandler(itsExceptionHandler);

    itsPlugMap[msgId] = std::move(p);

    itsDispatcher.NewPlug(msgId);
}


IWinMsgPlug* C::Impl::GetWinMsgPlug(unsigned int msgId) const
{
    if (auto i = itsPlugMap.find(msgId); i != end(itsPlugMap))
        return i->second.get();

    return nullptr;
}


void C::Impl::RegisterSpy(unsigned int msgId,
    const std::shared_ptr<ISpyWinMsgPlug>& s)
{
    D1_ASSERT(s);

    itsSpyPlugs.push_back(s);

    itsDispatcher.NewSpyPlug(msgId, s);
}


C::ProcRegistrar(
    IWinMsgDispatcher& d, IExceptionHandler* eh):
    itsImpl{ std::make_unique<Impl>(d, this, eh) }
{
}


C::~ProcRegistrar() = default;


void C::Register(
    unsigned int msgId, std::unique_ptr<IWinMsgPlug> p)
{
    itsImpl->Register(msgId, std::move(p));
}


IWinMsgPlug* C::GetWinMsgPlug(unsigned int msgId) const
{
    return itsImpl->GetWinMsgPlug(msgId);
}


void C::RegisterSpy(
    unsigned int msgId, const std::shared_ptr<ISpyWinMsgPlug>& s)
{
    itsImpl->RegisterSpy(msgId, s);
}

}
