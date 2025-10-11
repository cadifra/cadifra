/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

module WinUtil;

import std;


namespace WinUtil
{

namespace
{

class DeallocIdMsg: public WinMsgWrapper
{
public:
    DeallocIdMsg(WinMsg& m):
        WinMsgWrapper{ m, GetMsgId() }
    {
    }

    unsigned int Id() const { return static_cast<unsigned int>(GetWParam()); }

    static UINT GetMsgId(); // message constant

    static void Post(HWND window, unsigned int id);
};


UINT DeallocIdMsg::GetMsgId()
{
    static unsigned int m = WinUtil::PrivateMessage::Instance().Register();
    return m;
}


void DeallocIdMsg::Post(HWND window, unsigned int id)
{
    D1_VERIFY(::PostMessage(window, GetMsgId(), id, 0));
}

using C = Timer;
}


class C::Impl: private WinUtil::Window
{
    using This = Impl;

    ProcRegistrar itsProcReg;
    void OnTimer(WM_TIMER_Msg);
    void OnDeallocId(DeallocIdMsg);

    using Ids = std::set<unsigned int>;
    Ids itsUnusedIds;
    unsigned int itsMaxId = 0;

    using IdMap = std::map<unsigned int, WinUtil::ITimerClient*>;
    IdMap itsIdMap;

public:
    Impl();

    unsigned int SetTimer(
        WinUtil::ITimerClient*, unsigned int milliseconds); // returns id

    void KillTimer(unsigned int& id);
    // Kills the timer and sets id = 0
};


C::Impl::Impl():
    itsProcReg{ GetMsgDispatcher(), 0 }
{
    auto ph = itsProcReg.Helper(*this);

    ph.Register(&This::OnTimer);
    ph.Register(&This::OnDeallocId);

    HINSTANCE i = ::GetModuleHandle(0);

    static auto wc = WinUtil::WindowClass{
        TEXT("Cadifra Timer"),       // lpszClassName
        &WinUtil::WindowStartupProc, // lpfnWndProc
        i                            // hInstance
    };

    ::CreateWindow(
        wc.GetAtom(),                       // lpClassName
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


void C::Impl::OnTimer(WM_TIMER_Msg m)
{
    auto i = itsIdMap.find(m.wTimerID());

    if (i != end(itsIdMap))
        i->second->TimerElapsed();
}


void C::Impl::OnDeallocId(DeallocIdMsg m)
{
    itsUnusedIds.insert(m.Id());
}


void C::Impl::KillTimer(unsigned int& id)
{
    D1_ASSERT(itsIdMap.find(id) != end(itsIdMap));
    itsIdMap.erase(id);

    D1_VERIFY(::KillTimer(
        GetWindowHandle(),
        id));

    DeallocIdMsg::Post(GetWindowHandle(), id);
    // Posting a message guarantees that the id is not deallocated
    // while a WM_TIMER message using this id is still in the message
    // queue (this situation is possible when you call KillTimer).

    id = 0;
}


unsigned int C::Impl::SetTimer(ITimerClient* c, unsigned int milliseconds)
{
    D1_ASSERT(c);
    unsigned int id = 0;

    if (itsUnusedIds.size())
    {
        id = *begin(itsUnusedIds);
        itsUnusedIds.erase(begin(itsUnusedIds));
    }
    else
        id = ++itsMaxId;

    D1_ASSERT(id);

    const bool inserted = itsIdMap.insert(std::make_pair(id, c)).second;
    D1_ASSERT(inserted);

    D1_VERIFY(::SetTimer(
        GetWindowHandle(),
        id,
        milliseconds,
        0));

    return id;
}


auto C::Instance() -> Impl&
{
    static Impl singleton;
    return singleton;
}


C::Timer(ITimerClient& c):
    itsClient{ c }
{
}


C::Timer(ITimerClient& c, unsigned int milliseconds):
    itsClient{ c }
{
    Start(milliseconds);
}


C::~Timer()
{
    Stop();
}


void C::Start(unsigned int milliseconds)
{
    Stop();
    itsId = Instance().SetTimer(&itsClient, milliseconds);
    itIsRunning = true;
}


void C::Stop()
{
    if (itIsRunning)
    {
        Instance().KillTimer(itsId),
            itIsRunning = false;
    }
}

}
