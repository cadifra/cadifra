/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

/*
The ProcRegistrar makes the registration of IWinMsgPlugs in a
IMessageDispatcher.

The ProcRegistrar owns the registered IWinMsgPlugs.
In the destructor, the ProcRegistrar cancels the registration of
these IWinMsgPlugs and deletes them.
*/

export module WinUtil.ProcRegistrar;

import WinUtil.Message;

import std;


namespace WinUtil
{

export class ProcRegistrar;


export class IExceptionHandler
{
public:
    virtual void HandleException() = 0;
    // PRECONDITION: uncaught_exception() == TRUE

protected:
    ~IExceptionHandler() = default;
};


export class IMessagePlug
{
    const bool itIsAlwaysReady = false;
    IExceptionHandler* itsExceptionHandler = nullptr; // may be zero, no ownership

public:
    IMessagePlug(bool isAlwaysReady):
        itIsAlwaysReady{ isAlwaysReady }
    {
    }

    IMessagePlug(const IMessagePlug&) = delete;
    IMessagePlug& operator=(const IMessagePlug&) = delete;

    virtual ~IMessagePlug() = default;

    bool IsAlwaysReady() const { return itIsAlwaysReady; }

    void Process(Message&);

    void SetExceptionHandler(IExceptionHandler* eh)
    {
        itsExceptionHandler = eh;
    }

private:
    virtual void ProcessImp(Message&) = 0;
};


export class ISpyMessagePlug
{
public:
    ISpyMessagePlug() = default;

    ISpyMessagePlug(const ISpyMessagePlug&) = delete;
    ISpyMessagePlug& operator=(const ISpyMessagePlug&) = delete;

    virtual ~ISpyMessagePlug() = default;

    virtual void Spy(Message&) const = 0;
};


export class IMessageDispatcher
{
public:
    IMessageDispatcher() = default;
    virtual ~IMessageDispatcher() = default;

    virtual void Register(ProcRegistrar*) = 0;
    virtual void Unregister(ProcRegistrar*) = 0;

    virtual void NewPlug(unsigned int msgId) = 0;
    virtual void NewSpyPlug(unsigned int msgId,
        const std::weak_ptr<ISpyMessagePlug>&) = 0;

    IMessageDispatcher(const IMessageDispatcher&) = delete;
    IMessageDispatcher& operator=(const IMessageDispatcher&) = delete;
};


export class ProcRegistrar
{
public:
    ProcRegistrar(IMessageDispatcher&, IExceptionHandler* eh);

    ProcRegistrar(const ProcRegistrar&) = delete;
    ProcRegistrar& operator=(const ProcRegistrar&) = delete;

    ~ProcRegistrar(); // intentionally not virtual

    void Register(unsigned int msgId, std::unique_ptr<IMessagePlug>);

    IMessagePlug* GetMessagePlug(unsigned int msgId) const;

    void RegisterSpy(unsigned int msgId, const std::shared_ptr<ISpyMessagePlug>&);

    template <class Target>
    auto Helper(Target& t);

private:
    class Impl;
    const std::unique_ptr<Impl> itsImpl;
};


template <class Target, class MessageWrapper>
auto w_plug(bool isAlwaysReady, Target& t, void (Target::*p)(MessageWrapper))
{
    class Plug: public IMessagePlug
    {
        using ProcessFun = void (Target::*)(MessageWrapper);
        Target& itsTarget;
        const ProcessFun itsProcessFun;

        //-- IMessagePlug

        void ProcessImp(Message& msg) override
        {
            std::invoke(itsProcessFun, itsTarget, msg);
        }

        //--

    public:
        Plug(bool isAlwaysReady, Target& t, ProcessFun p):
            IMessagePlug{ isAlwaysReady },
            itsTarget{ t },
            itsProcessFun{ p }
        {
        }
    };

    return std::make_unique<Plug>(isAlwaysReady, t, p);
}


template <class Target, class MessageWrapper>
auto spy_plug(Target& t, void (Target::*p)(MessageWrapper))
{
    class Plug: public ISpyMessagePlug
    {
        using SpyFun = void (Target::*)(MessageWrapper);
        Target& itsTarget;
        const SpyFun itsSpyFun;

        void Spy(Message& msg) const override
        {
            std::invoke(itsSpyFun, itsTarget, msg);
        }

    public:
        Plug(Target& t, SpyFun p):
            itsTarget{ t }, itsSpyFun{ p }
        {
        }
    };

    return std::make_shared<Plug>(t, p);
}


template <class Target>
class HelperType
{
    ProcRegistrar& itsRegistrar;
    Target& itsTarget;

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
auto ProcRegistrar::Helper(Target& t)
{
    return HelperType<Target>{ *this, t };
}

}


/*
Register
========
An IMessageDispatcher D1 sends a windows message M1 (WM_MOUSEMOVE etc.) only to the
function f1 registered via the ProcRegistrar R1 if:

  - f1 is registered for M1
  - R1 was the last instantiated ProcRegistrar for the IMessageDispatcher D1.


RegisterAlwaysReady
===================
An IMessageDispatcher D1 sends a windows message M1 (WM_MOUSEMOVE etc.) only to the
function f1 registered via the ProcRegistrar R1 if:

  - f1 is registered for M1
  - No ProcRegistrar that was instantiated for the IMessageDispatcher D1 after
    the instantiation of R1, has registered a function for M1.


RegisterSpy
===========
An IMessageDispatcher sends a windows message first to all functions registered
with RegisterSpy before it calls any function registered with "Register" or
"RegisterAlwaysReady".

"RegisterSpy" doesn't influence the rules for message dispatching for functions
registered with "Register" or "RegisterAlwaysReady".

The function that is registered last via "RegisterSpy" will get the windows
message before any other functions registered via "RegisterSpy".
*/
