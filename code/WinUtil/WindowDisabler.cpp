/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil;

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
    itsOldActive{ ::GetActiveWindow() },
    itsProcessId{ ::GetCurrentProcessId() },
    itsListPos{ end(theDisablerList) }
{
    auto guard = d1::ScopeGuard{ [=] { Enable(); } };

    itsListPos = theDisablerList.insert(begin(theDisablerList), this);

    ::EnumThreadWindows(::GetCurrentThreadId(),
        reinterpret_cast<WNDENUMPROC>(&EnumWindowsProcNoThrow),
        reinterpret_cast<LPARAM>(this)
    );

    guard.Dismiss();
}


C::~WindowDisabler()
{
    Enable();
}


void C::Enable()
{
    if (itsListPos == end(theDisablerList))
        return;

    theDisablerList.erase(itsListPos);
    itsListPos = end(theDisablerList);

    for (HWND w : itsWindows)
    {
        ::SetWindowLongPtr(w, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(theMap.Get(w)));
        theMap.Erase(w);
        ::EnableWindow(w, TRUE);
    }

    if (!theDisablerList.empty())
    {
        HWND w = theDisablerList.front()->itsOldActive;
        HWND p = ::GetLastActivePopup(w);
        ::SetActiveWindow(p);
    }
}


BOOL CALLBACK C::EnumWindowsProcNoThrow(HWND hwnd, LPARAM lParam)
{
    return GuardedCallHelpers::call("WindowDisabler::EnumWindowsProc",
        &WindowDisabler::EnumWindowsProc, hwnd, lParam, FALSE);
}


BOOL C::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    auto* wd = reinterpret_cast<WindowDisabler*>(lParam);
    if (!wd)
        return FALSE;

    if (::IsWindowVisible(hwnd) && 0 == theMap.Get(hwnd))
    {
        wd->itsWindows.push_back(hwnd);
        ::EnableWindow(hwnd, FALSE);

        theMap.Insert(hwnd, reinterpret_cast<WNDPROC>(
                                ::GetWindowLongPtr(hwnd, GWLP_WNDPROC)));

        ::SetWindowLongPtr(
            hwnd,
            GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(&SubClassWindowProcNoThrow));
    }

    return TRUE;
}


void C::AddAndDisableIfNeeded(HWND hwnd)
{
    if (theDisablerList.empty())
        return;

    if (theMap.Get(hwnd))
        return;

    WindowDisabler* wd = theDisablerList.back();

    wd->itsWindows.push_back(hwnd);
    ::EnableWindow(hwnd, FALSE);

    theMap.Insert(hwnd, reinterpret_cast<WNDPROC>(
                            ::GetWindowLongPtr(hwnd, GWLP_WNDPROC)));

    ::SetWindowLongPtr(
        hwnd,
        GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(&SubClassWindowProcNoThrow));
}


LRESULT CALLBACK C::SubClassWindowProcNoThrow(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return GuardedCallHelpers::call("WindowDisabler::SubClassWindowProc",
        &WindowDisabler::SubClassWindowProc, hwnd, uMsg, wParam, lParam,
        static_cast<LRESULT>(0));
}


LRESULT C::SubClassWindowProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static unsigned int msgId = PrivateMessage::Instance().Register();

    if (uMsg == WM_ACTIVATE)
    {
        if ((wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) &&
            !theDisablerList.empty())
        {
            HWND w = theDisablerList.front()->itsOldActive;
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

    return CallWindowProc(theMap.Get(hwnd), hwnd, uMsg, wParam, lParam);
}

}
