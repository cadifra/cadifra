/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

module WinUtil.Window;

import WinUtil.CursorManager;
import WinUtil.GuardedFunctionCall;
import WinUtil.IWindow;


namespace WinUtil
{

namespace
{

WindowHandleTable<Window> sharedWindowHandleTable;

using C = Window;
}


C::Window():
    itsDefaultWindowProc{ &::DefWindowProc }
{
}


C::~Window()
{
    if (itsWindowHandle && !itsWindowIsDestroyed)
        ::DestroyWindow(itsWindowHandle);
}


void C::ProcessMsg(Message& msg) const
{
    class WaitCursorSwitcher: public WinUtil::IPrePostDispatchObserver
    {
        CursorManager::WaitCursorSwitch itsSwitch;
        void PreDispatchNotification() final { itsSwitch.On(); }
        void PostDispatchNotification() final { itsSwitch.Off(); }
    };

    auto wcs = WaitCursorSwitcher{};

    const HWND h = itsWindowHandle;
    const WNDPROC p = itsDefaultWindowProc;

    itsDispatcher.Dispatch(msg, wcs); // may delete this!

    if (msg.OSisEnabled() && !msg.DefProcCalled())
    {
        msg.DefProcCalled(::CallWindowProc(p,
            h, msg.GetMsgId(), msg.GetWParam(), msg.GetLParam()));
    }
}


auto C::GetDispatcher() const -> IDispatcher&
{
    return itsDispatcher;
}


void C::CallDefProcNow(Message& msg) const
{
    msg.DefProcCalled(::CallWindowProc(itsDefaultWindowProc,
        itsWindowHandle, msg.GetMsgId(), msg.GetWParam(), msg.GetLParam()));
}


void C::SetDefaultWindowProc(WNDPROC p)
{
    itsDefaultWindowProc = p;
}


void C::SubClassWindow(HWND hwnd)
{
    D1_ASSERT(!itsWindowIsDestroyed);
    itsWindowHandle = hwnd;
    itsRemover = sharedWindowHandleTable.Add(hwnd, this);

    itsDefaultWindowProc =
        reinterpret_cast<WNDPROC>(
            ::SetWindowLongPtr(
                hwnd,
                GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(SharedWindowProc)));
}


LRESULT C::WindowProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    itsWindowHandle = hwnd;

    if (uMsg == WM_DESTROY)
    {
        D1_ASSERT(!itsWindowIsDestroyed);
        itsWindowIsDestroyed = true;
    }
    else if (uMsg == WM_NCDESTROY)
        itsRemover.reset();

    auto msg = Message{ uMsg, wp, lp };

    ProcessMsg(msg);

    return msg.GetLResult();
}


LRESULT CALLBACK C::SharedWindowProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    Window* w = sharedWindowHandleTable.Find(hwnd);

    if (!w)
        return ::DefWindowProc(hwnd, uMsg, wp, lp);

    return GuardedCallHelpers::call(
        "WinUtil::Window",
        *w,
        &Window::WindowProc,
        hwnd, uMsg, wp, lp,
        LRESULT(0));
}

}
