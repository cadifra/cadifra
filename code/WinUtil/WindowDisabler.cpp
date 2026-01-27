/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.Window;

import WinUtil.GuardedFunctionCall;
import WinUtil.MessageLoop;

import d1.ScopeGuard;
import d1.VectorMap;


namespace WinUtil
{

namespace
{
d1::VectorMap<HWND, WNDPROC> theMap;

using C = WindowDisabler;
}


C::DisablerList C::theDisablerList;


C::WindowDisabler():
    oldActive_{ ::GetActiveWindow() },
    processId_{ ::GetCurrentProcessId() },
    listPos_{ end(theDisablerList) }
{
    auto guard = d1::ScopeGuard{ [=] { enable(); } };

    listPos_ = theDisablerList.insert(begin(theDisablerList), this);

    ::EnumThreadWindows(::GetCurrentThreadId(),
        reinterpret_cast<WNDENUMPROC>(&enumWindowsProcNoThrow),
        reinterpret_cast<LPARAM>(this)
    );

    guard.dismiss();
}


C::~WindowDisabler()
{
    enable();
}


void C::enable()
{
    if (listPos_ == end(theDisablerList))
        return;

    theDisablerList.erase(listPos_);
    listPos_ = end(theDisablerList);

    for (HWND w : windows_)
    {
        ::SetWindowLongPtr(w, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(theMap.get(w)));
        theMap.erase(w);
        ::EnableWindow(w, TRUE);
    }

    if (not theDisablerList.empty())
    {
        HWND w = theDisablerList.front()->oldActive_;
        HWND p = ::GetLastActivePopup(w);
        ::SetActiveWindow(p);
    }
}


BOOL CALLBACK C::enumWindowsProcNoThrow(HWND hwnd, LPARAM lParam)
{
    return GuardedCallHelpers::call("WindowDisabler::EnumWindowsProc",
        &WindowDisabler::enumWindowsProc, hwnd, lParam, FALSE);
}


BOOL C::enumWindowsProc(HWND hwnd, LPARAM lParam)
{
    auto* wd = reinterpret_cast<WindowDisabler*>(lParam);
    if (not wd)
        return FALSE;

    if (::IsWindowVisible(hwnd) and 0 == theMap.get(hwnd))
    {
        wd->windows_.push_back(hwnd);
        ::EnableWindow(hwnd, FALSE);

        theMap.insert(hwnd, reinterpret_cast<WNDPROC>(
                                ::GetWindowLongPtr(hwnd, GWLP_WNDPROC)));

        ::SetWindowLongPtr(
            hwnd,
            GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(&subClassWindowProcNoThrow));
    }

    return TRUE;
}


void C::addAndDisableIfNeeded(HWND hwnd)
{
    if (theDisablerList.empty())
        return;

    if (theMap.get(hwnd))
        return;

    WindowDisabler* wd = theDisablerList.back();

    wd->windows_.push_back(hwnd);
    ::EnableWindow(hwnd, FALSE);

    theMap.insert(hwnd, reinterpret_cast<WNDPROC>(
                            ::GetWindowLongPtr(hwnd, GWLP_WNDPROC)));

    ::SetWindowLongPtr(
        hwnd,
        GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(&subClassWindowProcNoThrow));
}


LRESULT CALLBACK C::subClassWindowProcNoThrow(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return GuardedCallHelpers::call("WindowDisabler::SubClassWindowProc",
        &WindowDisabler::subClassWindowProc, hwnd, uMsg, wParam, lParam,
        static_cast<LRESULT>(0));
}


LRESULT C::subClassWindowProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static unsigned int msgId = PrivateMessage::instance().getNumber();

    if (uMsg == WM_ACTIVATE)
    {
        if ((wParam == WA_ACTIVE or wParam == WA_CLICKACTIVE) and
            not theDisablerList.empty())
        {
            HWND w = theDisablerList.front()->oldActive_;
            ::PostMessage(
                hwnd,
                msgId,
                reinterpret_cast<WPARAM>(w),
                0
            );
        }
    }
    else if (uMsg == msgId)
    {
        HWND w = reinterpret_cast<HWND>(wParam);
        HWND p = ::GetLastActivePopup(w);
        ::SetActiveWindow(p);
    }

    return CallWindowProc(theMap.get(hwnd), hwnd, uMsg, wParam, lParam);
}

}
