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
    defaultWindowProc_{ &::DefWindowProc }
{
}


C::~Window()
{
    if (windowHandle_ and not windowIsDestroyed_)
        ::DestroyWindow(windowHandle_);
}


void C::processMsg(Message& msg) const
{
    class WaitCursorSwitcher: public WinUtil::IPrePostDispatchObserver
    {
        CursorManager::WaitCursorSwitch switch_;
        void preDispatchNotification() final { switch_.on(); }
        void postDispatchNotification() final { switch_.off(); }
    };

    auto wcs = WaitCursorSwitcher{};

    const HWND h = windowHandle_;
    const WNDPROC p = defaultWindowProc_;

    dispatcher_.dispatch(msg, wcs); // may delete this!

    if (msg.OSisEnabled() and not msg.defProcCalled())
    {
        msg.defProcCalled(::CallWindowProc(p,
            h, msg.getMsgId(), msg.getWParam(), msg.getLParam()));
    }
}


auto C::getDispatcher() const -> IDispatcher&
{
    return dispatcher_;
}


void C::callDefProcNow(Message& msg) const
{
    msg.defProcCalled(::CallWindowProc(defaultWindowProc_,
        windowHandle_, msg.getMsgId(), msg.getWParam(), msg.getLParam()));
}


void C::setDefaultWindowProc(WNDPROC p)
{
    defaultWindowProc_ = p;
}


void C::subClassWindow(HWND hwnd)
{
    D1_ASSERT(not windowIsDestroyed_);
    windowHandle_ = hwnd;
    remover_ = sharedWindowHandleTable.add(hwnd, this);

    defaultWindowProc_ =
        reinterpret_cast<WNDPROC>(
            ::SetWindowLongPtr(
                hwnd,
                GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(sharedWindowProc)));
}


LRESULT C::windowProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    windowHandle_ = hwnd;

    if (uMsg == WM_DESTROY)
    {
        D1_ASSERT(not windowIsDestroyed_);
        windowIsDestroyed_ = true;
    }
    else if (uMsg == WM_NCDESTROY)
        remover_.reset();

    auto msg = Message{ uMsg, wp, lp };

    processMsg(msg);

    return msg.getLResult();
}


LRESULT CALLBACK C::sharedWindowProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    Window* w = sharedWindowHandleTable.find(hwnd);

    if (not w)
        return ::DefWindowProc(hwnd, uMsg, wp, lp);

    return GuardedCallHelpers::call(
        "WinUtil::Window",
        *w,
        &Window::windowProc,
        hwnd, uMsg, wp, lp,
        LRESULT(0));
}

}
