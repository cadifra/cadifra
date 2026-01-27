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
    virtual void handleException() = 0;
    // PRECONDITION: uncaught_exception() == TRUE

protected:
    ~IExceptionHandler() = default;
};


class IProcessor
{
    const bool alwaysReady_ = false;
    IExceptionHandler* exceptionHandler_ = nullptr; // may be zero, no ownership

public:
    IProcessor(bool alwaysReady):
        alwaysReady_{ alwaysReady }
    {
    }

    IProcessor(const IProcessor&) = delete;
    IProcessor& operator=(const IProcessor&) = delete;

    virtual ~IProcessor() = default;

    bool isAlwaysReady() const { return alwaysReady_; }

    void process(Message&);

    void setExceptionHandler(IExceptionHandler* eh)
    {
        exceptionHandler_ = eh;
    }

private:
    virtual void processImp(Message&) = 0;
};


class ISpy
{
public:
    ISpy() = default;

    ISpy(const ISpy&) = delete;
    ISpy& operator=(const ISpy&) = delete;

    virtual ~ISpy() = default;

    virtual void spy(Message&) const = 0;
};


export class IDispatcher
{
public:
    IDispatcher() = default;
    virtual ~IDispatcher() = default;

    virtual void add(ProcRegistrar*) = 0;
    virtual void forget(ProcRegistrar*) = 0;

    virtual void newProcessor(unsigned int msgId) = 0;
    virtual void newSpy(unsigned int msgId, const std::weak_ptr<ISpy>&) = 0;

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

    void add(unsigned int msgId, std::unique_ptr<IProcessor>);

    IProcessor* getProcessor(unsigned int msgId) const;

    void add(unsigned int msgId, const std::shared_ptr<ISpy>&);

    template <class Target>
    auto helper(Target& t);

private:
    class Impl;
    const std::unique_ptr<Impl> impl_;
};


template <class Target, class MessageWrapper>
auto make_processor(bool isAlwaysReady, Target& t, void (Target::*p)(MessageWrapper))
{
    class ProcessorImp: public IProcessor
    {
        using ProcessFun = void (Target::*)(MessageWrapper);
        Target& target_;
        const ProcessFun processFun_;

        //-- IProcessor

        void processImp(Message& msg) override
        {
            std::invoke(processFun_, target_, msg);
        }

        //--

    public:
        ProcessorImp(bool isAlwaysReady, Target& t, ProcessFun p):
            IProcessor{ isAlwaysReady },
            target_{ t },
            processFun_{ p }
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
        Target& target_;
        const SpyFun spyFun_;

        void spy(Message& msg) const override
        {
            std::invoke(spyFun_, target_, msg);
        }

    public:
        SpyImp(Target& t, SpyFun p):
            target_{ t }, spyFun_{ p }
        {
        }
    };

    return std::make_shared<SpyImp>(t, p);
}


template <class Target>
class HelperType
{
    ProcRegistrar& registrar_;
    Target& target_;

public:
    HelperType(ProcRegistrar& r, Target& t):
        registrar_{ r }, target_{ t }
    {
    }

    template <class Target, class MessageWrapper>
    using MessageMemfun = void (Target::*)(MessageWrapper);

    template <class MessageWrapper>
    void add(MessageMemfun<Target, MessageWrapper> f)
    {
        auto p = make_processor(false, target_, f);
        registrar_.add(MessageWrapper::getMsgId(), std::move(p));
    }

    template <class MessageWrapper>
    void addAlwaysReady(MessageMemfun<Target, MessageWrapper> f)
    {
        auto p = make_processor(true, target_, f);
        registrar_.add(MessageWrapper::getMsgId(), std::move(p));
    }

    template <class MessageWrapper>
    void addSpy(MessageMemfun<Target, MessageWrapper> f)
    {
        auto p = make_spy(target_, f);
        registrar_.add(MessageWrapper::getMsgId(), p);
    }
};


template <class Target>
auto ProcRegistrar::helper(Target& t)
{
    return HelperType{ *this, t };
}


export class IPrePostDispatchObserver
{
public:
    virtual void preDispatchNotification() = 0;
    // PreDispatchNotification is called before any function registered
    // for the dispatched message is called.

    virtual void postDispatchNotification() = 0;
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

    void dispatch(Message&, IPrePostDispatchObserver&) const;

private:
    class Impl;
    std::shared_ptr<Impl> impl_;
};

}


/*
add
===
An IDispatcher D1 sends a windows message M1 (WM_MOUSEMOVE etc.) only to the
function f1 registered via the ProcRegistrar R1 if:

  - f1 is registered for M1
  - R1 was the last instantiated ProcRegistrar for the IDispatcher D1.


addAlwaysReady
==============
An IDispatcher D1 sends a windows message M1 (WM_MOUSEMOVE etc.) only to the
function f1 registered via the ProcRegistrar R1 if:

  - f1 is registered for M1
  - No ProcRegistrar that was instantiated for the IDispatcher D1 after
    the instantiation of R1, has registered a function for M1.


addSpy
======
An IDispatcher sends a windows message first to all functions registered
with addSpy before it calls any function registered with "add" or
"addAlwaysReady".

"addSpy" doesn't influence the rules for message dispatching for functions
registered with "add" or "addAlwaysReady".

The function that is registered last via "addSpy" will get the windows
message before any other functions registered via "addSpy".
*/
