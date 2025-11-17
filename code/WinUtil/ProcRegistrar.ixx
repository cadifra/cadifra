/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

/*
The ProcRegistrar makes the registration of IProcessor in a
IDispatcher.

The ProcRegistrar owns the registered IProcessor.
In the destructor, the ProcRegistrar cancels the registration of
these IProcessor and deletes them.
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


class IProcessor
{
    const bool itIsAlwaysReady = false;
    IExceptionHandler* itsExceptionHandler = nullptr; // may be zero, no ownership

public:
    IProcessor(bool isAlwaysReady):
        itIsAlwaysReady{ isAlwaysReady }
    {
    }

    IProcessor(const IProcessor&) = delete;
    IProcessor& operator=(const IProcessor&) = delete;

    virtual ~IProcessor() = default;

    bool IsAlwaysReady() const { return itIsAlwaysReady; }

    void Process(Message&);

    void SetExceptionHandler(IExceptionHandler* eh)
    {
        itsExceptionHandler = eh;
    }

private:
    virtual void ProcessImp(Message&) = 0;
};


class ISpy
{
public:
    ISpy() = default;

    ISpy(const ISpy&) = delete;
    ISpy& operator=(const ISpy&) = delete;

    virtual ~ISpy() = default;

    virtual void Spy(Message&) const = 0;
};


export class IDispatcher
{
public:
    IDispatcher() = default;
    virtual ~IDispatcher() = default;

    virtual void Register(ProcRegistrar*) = 0;
    virtual void Unregister(ProcRegistrar*) = 0;

    virtual void NewProcessor(unsigned int msgId) = 0;
    virtual void NewSpy(unsigned int msgId, const std::weak_ptr<ISpy>&) = 0;

    IDispatcher(const IDispatcher&) = delete;
    IDispatcher& operator=(const IDispatcher&) = delete;
};


export class ProcRegistrar
{
public:
    ProcRegistrar(IDispatcher&, IExceptionHandler* eh);

    ProcRegistrar(const ProcRegistrar&) = delete;
    ProcRegistrar& operator=(const ProcRegistrar&) = delete;

    ~ProcRegistrar(); // intentionally not virtual

    void Register(unsigned int msgId, std::unique_ptr<IProcessor>);

    IProcessor* GetProcessor(unsigned int msgId) const;

    void RegisterSpy(unsigned int msgId, const std::shared_ptr<ISpy>&);

    template <class Target>
    auto Helper(Target& t);

private:
    class Impl;
    const std::unique_ptr<Impl> itsImpl;
};


template <class Target, class MessageWrapper>
auto make_processor(bool isAlwaysReady, Target& t, void (Target::*p)(MessageWrapper))
{
    class ProcessorImp: public IProcessor
    {
        using ProcessFun = void (Target::*)(MessageWrapper);
        Target& itsTarget;
        const ProcessFun itsProcessFun;

        //-- IProcessor

        void ProcessImp(Message& msg) override
        {
            std::invoke(itsProcessFun, itsTarget, msg);
        }

        //--

    public:
        ProcessorImp(bool isAlwaysReady, Target& t, ProcessFun p):
            IProcessor{ isAlwaysReady },
            itsTarget{ t },
            itsProcessFun{ p }
        {
        }
    };

    return std::make_unique<ProcessorImp>(isAlwaysReady, t, p);
}


template <class Target, class MessageWrapper>
auto make_spy(Target& t, void (Target::*p)(MessageWrapper))
{
    class SpyImp: public ISpy
    {
        using SpyFun = void (Target::*)(MessageWrapper);
        Target& itsTarget;
        const SpyFun itsSpyFun;

        void Spy(Message& msg) const override
        {
            std::invoke(itsSpyFun, itsTarget, msg);
        }

    public:
        SpyImp(Target& t, SpyFun p):
            itsTarget{ t }, itsSpyFun{ p }
        {
        }
    };

    return std::make_shared<SpyImp>(t, p);
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
        auto p = make_processor(false, itsTarget, f);
        itsRegistrar.Register(MessageWrapper::GetMsgId(), std::move(p));
    }

    template <class MessageWrapper>
    void RegisterAlwaysReady(MessageMemfun<Target, MessageWrapper> f)
    {
        auto p = make_processor(true, itsTarget, f);
        itsRegistrar.Register(MessageWrapper::GetMsgId(), std::move(p));
    }

    template <class MessageWrapper>
    void RegisterSpy(MessageMemfun<Target, MessageWrapper> f)
    {
        auto p = make_spy(itsTarget, f);
        itsRegistrar.RegisterSpy(MessageWrapper::GetMsgId(), p);
    }
};


template <class Target>
auto ProcRegistrar::Helper(Target& t)
{
    return HelperType<Target>{ *this, t };
}


export class IPrePostDispatchObserver
{
public:
    virtual void PreDispatchNotification() = 0;
    // PreDispatchNotification is called before any function registered
    // for the dispatched message is called.

    virtual void PostDispatchNotification() = 0;
    // PostDispatchNotification is called after the last function registered
    // for the dispatched message has called.
    // PostDispatchNotification is never called if not PreDispatchNotification
    // has been called before.
    // PostDispatchNotification is guaranteed to be called if
    // PreDispatchNotification has been called before (also if an exception
    // occurs).

protected:
    ~IPrePostDispatchObserver() = default;
};


export class Dispatcher
{
public:
    Dispatcher();

    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;

    ~Dispatcher(); // intentionally not virtual

    operator IDispatcher&() const;

    void Dispatch(Message&, IPrePostDispatchObserver&) const;

private:
    class Impl;
    std::shared_ptr<Impl> itsImpl;
};

}


/*
Register
========
An IDispatcher D1 sends a windows message M1 (WM_MOUSEMOVE etc.) only to the
function f1 registered via the ProcRegistrar R1 if:

  - f1 is registered for M1
  - R1 was the last instantiated ProcRegistrar for the IDispatcher D1.


RegisterAlwaysReady
===================
An IDispatcher D1 sends a windows message M1 (WM_MOUSEMOVE etc.) only to the
function f1 registered via the ProcRegistrar R1 if:

  - f1 is registered for M1
  - No ProcRegistrar that was instantiated for the IDispatcher D1 after
    the instantiation of R1, has registered a function for M1.


RegisterSpy
===========
An IDispatcher sends a windows message first to all functions registered
with RegisterSpy before it calls any function registered with "Register" or
"RegisterAlwaysReady".

"RegisterSpy" doesn't influence the rules for message dispatching for functions
registered with "Register" or "RegisterAlwaysReady".

The function that is registered last via "RegisterSpy" will get the windows
message before any other functions registered via "RegisterSpy".
*/
