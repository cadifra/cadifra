/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

export module WinUtil.Timer;

namespace WinUtil
{

export class Timer
{
public:
    class Client;

    Timer(Client&);
    // creates a timer without starting it

    Timer(Client&, unsigned int milliseconds);
    // creates a timer and calls Start

    ~Timer();
    // calls Stop
    // intentionally not virtual

    void start(unsigned int milliseconds);
    // calls Stop and starts the timer

    void stop();
    // stops the timer if it's running

    bool isRunning() const { return running_; }

private:
    class Impl;
    static Impl& instance();

    bool running_ = false;
    unsigned int id_ = 0;
    Client& client_;
};

// Timer uses windows messages. The ITimerClient::TimerElapsed function
// is only called while the windows message loop is working.


class Timer::Client
{
public:
    virtual void timerElapsed() = 0;

protected:
    ~Client() = default;
};

}


module : private;


import WinUtil.Window;
import WinUtil.MessageLoop;
import WinUtil.Messages;
import WinUtil.WindowClass;
import WinUtil.PrivateMessage;

import std;


namespace WinUtil
{

class DeallocIdMsg: public Message::Wrapper
{
public:
    DeallocIdMsg(Message& m):
        Wrapper{ m, getMsgId() }
    {
    }

    unsigned int id() const { return static_cast<unsigned int>(getWParam()); }

    static UINT getMsgId(); // message constant

    static void post(HWND window, unsigned int id);
};


UINT DeallocIdMsg::getMsgId()
{
    static unsigned int m = PrivateMessage::instance().getNumber();
    return m;
}


void DeallocIdMsg::post(HWND window, unsigned int id)
{
    D1_VERIFY(::PostMessage(window, getMsgId(), id, 0));
}


class Timer::Impl: private WinUtil::Window
{
    using This = Impl;

    ProcRegistrar procReg_;
    void onTimer(WM_TIMER_Msg);
    void onDeallocId(DeallocIdMsg);

    using Ids = std::set<unsigned int>;
    Ids unusedIds_;
    unsigned int maxId_ = 0;

    using IdMap = std::map<unsigned int, Client*>;
    IdMap idMap_;

public:
    Impl();

    unsigned int setTimer(
        Client*, unsigned int milliseconds); // returns id

    void killTimer(unsigned int& id);
    // Kills the timer and sets id = 0
};


Timer::Impl::Impl():
    procReg_{ getDispatcher(), 0 }
{
    auto ph = procReg_.helper(*this);

    ph.add(&This::onTimer);
    ph.add(&This::onDeallocId);

    HINSTANCE i = ::GetModuleHandle(0);

    static auto wc = WinUtil::WindowClass{
        TEXT("Cadifra Timer"),       // lpszClassName
        &WinUtil::windowStartupProc, // lpfnWndProc
        i                            // hInstance
    };

    ::CreateWindow(
        wc.getAtom(),                       // lpClassName
        0,                                  // lpWindowName
        WS_DISABLED,                        // dwStyle
        0, 0,                               // x, y
        0, 0,                               // nWidth, nHeight
        HWND_MESSAGE,                       // hWndParent
        0,                                  // hMenu
        i,                                  // hInstance
        static_cast<WinUtil::Window*>(this) // lpParam
    );
}


void Timer::Impl::onTimer(WM_TIMER_Msg m)
{
    auto i = idMap_.find(m.wTimerID());

    if (i != end(idMap_))
        i->second->timerElapsed();
}


void Timer::Impl::onDeallocId(DeallocIdMsg m)
{
    unusedIds_.insert(m.id());
}


void Timer::Impl::killTimer(unsigned int& id)
{
    D1_ASSERT(idMap_.find(id) != end(idMap_));
    idMap_.erase(id);

    D1_VERIFY(::KillTimer(
        getWindowHandle(),
        id));

    DeallocIdMsg::post(getWindowHandle(), id);
    // Posting a message guarantees that the id is not deallocated
    // while a WM_TIMER message using this id is still in the message
    // queue (this situation is possible when you call KillTimer).

    id = 0;
}


unsigned int Timer::Impl::setTimer(Client* c, unsigned int milliseconds)
{
    D1_ASSERT(c);
    unsigned int id = 0;

    if (unusedIds_.size())
    {
        id = *begin(unusedIds_);
        unusedIds_.erase(begin(unusedIds_));
    }
    else
        id = ++maxId_;

    D1_ASSERT(id);

    const bool inserted = idMap_.insert(std::pair{ id, c }).second;
    D1_ASSERT(inserted);

    D1_VERIFY(::SetTimer(
        getWindowHandle(),
        id,
        milliseconds,
        0));

    return id;
}


auto Timer::instance() -> Impl&
{
    static Impl singleton;
    return singleton;
}


Timer::Timer(Client& c):
    client_{ c }
{
}


Timer::Timer(Client& c, unsigned int milliseconds):
    client_{ c }
{
    start(milliseconds);
}


Timer::~Timer()
{
    stop();
}


void Timer::start(unsigned int milliseconds)
{
    stop();
    id_ = instance().setTimer(&client_, milliseconds);
    running_ = true;
}


void Timer::stop()
{
    if (running_)
    {
        instance().killTimer(id_),
            running_ = false;
    }
}

}
