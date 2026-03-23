/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

module WinUtil.Timer;

import WinUtil.IWindow;
import WinUtil.MessageLoop;
import WinUtil.Messages;
import WinUtil.Window;

import std;


namespace WinUtil
{

namespace
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
    static unsigned int m = WinUtil::PrivateMessage::instance().getNumber();
    return m;
}


void DeallocIdMsg::post(HWND window, unsigned int id)
{
    D1_VERIFY(::PostMessage(window, getMsgId(), id, 0));
}

using C = Timer;
}


class C::Impl: private WinUtil::Window
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


C::Impl::Impl():
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


void C::Impl::onTimer(WM_TIMER_Msg m)
{
    auto i = idMap_.find(m.wTimerID());

    if (i != end(idMap_))
        i->second->timerElapsed();
}


void C::Impl::onDeallocId(DeallocIdMsg m)
{
    unusedIds_.insert(m.id());
}


void C::Impl::killTimer(unsigned int& id)
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


unsigned int C::Impl::setTimer(Client* c, unsigned int milliseconds)
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


auto C::instance() -> Impl&
{
    static Impl singleton;
    return singleton;
}


C::Timer(Client& c):
    client_{ c }
{
}


C::Timer(Client& c, unsigned int milliseconds):
    client_{ c }
{
    start(milliseconds);
}


C::~Timer()
{
    stop();
}


void C::start(unsigned int milliseconds)
{
    stop();
    id_ = instance().setTimer(&client_, milliseconds);
    running_ = true;
}


void C::stop()
{
    if (running_)
    {
        instance().killTimer(id_),
            running_ = false;
    }
}

}
