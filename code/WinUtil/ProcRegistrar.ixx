/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

/*
The ProcRegistrar makes the registration of IWinMsgPlugs in a
IWinMsgDispatcher.

The ProcRegistrar owns the registered IWinMsgPlugs.
In the destructor, the ProcRegistrar cancels the registration of
these IWinMsgPlugs and deletes them.
*/

export module WinUtil:ProcRegistrar;

import :WinMsg;

import std;


namespace WinUtil
{

export class ProcRegistrar;

template <class Target>
class ProcRegistrarHelper;


export class IExceptionHandler
{
public:
    virtual void HandleException() = 0;
    // PRECONDITION: uncaught_exception() == TRUE

protected:
    ~IExceptionHandler() = default;
};


export class IWinMsgPlug
{
    const bool itIsAlwaysReady = false;
    IExceptionHandler* itsExceptionHandler = nullptr; // may be zero, no ownership

public:
    IWinMsgPlug(bool isAlwaysReady):
        itIsAlwaysReady{ isAlwaysReady }
    {
    }

    IWinMsgPlug(const IWinMsgPlug&) = delete;
    IWinMsgPlug& operator=(const IWinMsgPlug&) = delete;

    virtual ~IWinMsgPlug() = default;

    bool IsAlwaysReady() const { return itIsAlwaysReady; }

    void Process(WinMsg&);

    void SetExceptionHandler(IExceptionHandler* eh)
    {
        itsExceptionHandler = eh;
    }

private:
    virtual void ProcessImp(WinMsg&) = 0;
};


export class ISpyWinMsgPlug
{
public:
    ISpyWinMsgPlug() = default;

    ISpyWinMsgPlug(const ISpyWinMsgPlug&) = delete;
    ISpyWinMsgPlug& operator=(const ISpyWinMsgPlug&) = delete;

    virtual ~ISpyWinMsgPlug() = default;

    virtual void Spy(WinMsg&) const = 0;
};


export class IWinMsgDispatcher
{
public:
    IWinMsgDispatcher() = default;
    virtual ~IWinMsgDispatcher() = default;

    virtual void Register(ProcRegistrar*) = 0;
    virtual void Unregister(ProcRegistrar*) = 0;

    virtual void NewPlug(unsigned int msgId) = 0;
    virtual void NewSpyPlug(unsigned int msgId,
        const std::weak_ptr<ISpyWinMsgPlug>&) = 0;

    IWinMsgDispatcher(const IWinMsgDispatcher&) = delete;
    IWinMsgDispatcher& operator=(const IWinMsgDispatcher&) = delete;
};


export class ProcRegistrar
{
public:
    ProcRegistrar(IWinMsgDispatcher&, IExceptionHandler* eh);

    ProcRegistrar(const ProcRegistrar&) = delete;
    ProcRegistrar& operator=(const ProcRegistrar&) = delete;

    ~ProcRegistrar(); // intentionally not virtual

    void Register(unsigned int msgId, std::unique_ptr<IWinMsgPlug>);

    IWinMsgPlug* GetWinMsgPlug(unsigned int msgId) const;

    void RegisterSpy(unsigned int msgId, const std::shared_ptr<ISpyWinMsgPlug>&);

    template <class Target>
    class HelperType;

    template <class Target>
    auto Helper(Target& t) -> HelperType<Target>;

private:
    class Impl;
    const std::unique_ptr<Impl> itsImpl;
};


template <class Target, class MessageWrapper>
class WinMsgPlug: public IWinMsgPlug
{
    using ProcessFun = void (Target::*)(MessageWrapper);
    Target& itsTarget;
    const ProcessFun itsProcessFun;

    //-- IWinMsgPlug

    void ProcessImp(WinMsg& msg) override
    {
        std::invoke(itsProcessFun, itsTarget, msg);
    }

    //--

public:
    WinMsgPlug(bool isAlwaysReady, Target& t, ProcessFun p):
        IWinMsgPlug{ isAlwaysReady },
        itsTarget{ t },
        itsProcessFun{ p }
    {
    }
};


template <class Target, class MessageWrapper>
auto w_plug(bool isAlwaysReady, Target& t, void (Target::*p)(MessageWrapper))
    -> std::unique_ptr<WinMsgPlug<Target, MessageWrapper>>
{
    using Plug = WinMsgPlug<Target, MessageWrapper>;
    return std::make_unique<Plug>(isAlwaysReady, t, p);
}


template <class Target, class MessageWrapper>
class SpyWinMsgPlug: public ISpyWinMsgPlug
{
    using SpyFun = void (Target::*)(MessageWrapper);
    Target& itsTarget;
    const SpyFun itsSpyFun;

    void Spy(WinMsg& msg) const override
    {
        std::invoke(itsSpyFun, itsTarget, msg);
    }

public:
    SpyWinMsgPlug(Target& t, SpyFun p):
        itsTarget{ t }, itsSpyFun{ p }
    {
    }
};


template <class Target, class MessageWrapper>
auto spy_plug(Target& t, void (Target::*p)(MessageWrapper))
    -> std::shared_ptr<SpyWinMsgPlug<Target, MessageWrapper>>
{
    using Plug = SpyWinMsgPlug<Target, MessageWrapper>;
    return std::make_shared<Plug>(t, p);
}


template <class Target>
class ProcRegistrar::HelperType
{
    Target& itsTarget;
    ProcRegistrar& itsRegistrar;

public:
    HelperType(ProcRegistrar& r, Target& t):
        itsRegistrar{ r }, itsTarget{ t }
    {
    }

    template <class Target, class MessageWrapper>
    using MessageMemfun = void (Target::*)(MessageWrapper);

    template <class MessageWrapper>
    void Register(MessageMemfun<Target, MessageWrapper> f)
    {
        auto p = w_plug(false, itsTarget, f);
        itsRegistrar.Register(MessageWrapper::GetMsgId(), std::move(p));
    }

    template <class MessageWrapper>
    void RegisterAlwaysReady(MessageMemfun<Target, MessageWrapper> f)
    {
        auto p = w_plug(true, itsTarget, f);
        itsRegistrar.Register(MessageWrapper::GetMsgId(), std::move(p));
    }

    template <class MessageWrapper>
    void RegisterSpy(MessageMemfun<Target, MessageWrapper> f)
    {
        auto p = spy_plug(itsTarget, f);
        itsRegistrar.RegisterSpy(MessageWrapper::GetMsgId(), p);
    }
};


template <class Target>
auto ProcRegistrar::Helper(Target& t) -> HelperType<Target>
{
    return { *this, t };
}

}


/*
Register
========
An IWinMsgDispatcher D1 sends a windows message M1 (WM_MOUSEMOVE etc.) only to the
function f1 registered via the ProcRegistrar R1 if:

  - f1 is registered for M1
  - R1 was the last instantiated ProcRegistrar for the IWinMsgDispatcher D1.


RegisterAlwaysReady
===================
An IWinMsgDispatcher D1 sends a windows message M1 (WM_MOUSEMOVE etc.) only to the
function f1 registered via the ProcRegistrar R1 if:

  - f1 is registered for M1
  - No ProcRegistrar that was instantiated for the IWinMsgDispatcher D1 after
    the instantiation of R1, has registered a function for M1.


RegisterSpy
===========
An IWinMsgDispatcher sends a windows message first to all functions registered
with RegisterSpy before it calls any function registered with "Register" or
"RegisterAlwaysReady".

"RegisterSpy" doesn't influence the rules for message dispatching for functions
registered with "Register" or "RegisterAlwaysReady".

The function that is registered last via "RegisterSpy" will get the windows
message before any other functions registered via "RegisterSpy".
*/
