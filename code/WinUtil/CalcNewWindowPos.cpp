/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

module WinUtil.Window;


namespace WinUtil
{

namespace
{

void getMonitorSizeAndWorkArea(HWND w, RECT& monitor, RECT& work)
{
    monitor.left = 0;
    monitor.top = 0;
    monitor.right = GetSystemMetrics(SM_CXSCREEN);
    monitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    D1_ASSERT(monitor.right);
    D1_ASSERT(monitor.bottom);

    D1_VERIFY(::SystemParametersInfo(SPI_GETWORKAREA, 0, &work, 0));


    HMODULE dll = ::GetModuleHandle(TEXT("user32.dll")); // this module is always loaded

    if (not dll)
        return;

    using MFW = HMONITOR(WINAPI*)(HWND, DWORD);

    // the following call will fail on OS without multi monitor support
    MFW mfw = reinterpret_cast<MFW>(::GetProcAddress(dll, "MonitorFromWindow"));

    if (not mfw)
        return;

    HMONITOR hm = mfw(w, MONITOR_DEFAULTTONEAREST);

    if (not hm)
        return;

    using GMI = BOOL(WINAPI*)(HMONITOR, LPMONITORINFO);

    GMI gmi = reinterpret_cast<GMI>(::GetProcAddress(dll, "GetMonitorInfoA"));

    if (not gmi)
        return;

    auto info = MONITORINFO{};
    info.cbSize = sizeof(info);

    if (not gmi(hm, &info))
        return;

    work = info.rcWork;
    monitor = info.rcMonitor;
}

}


auto calcNewWindowPos(HWND w) -> WindowPlacement
{
    auto wp = WindowPlacement{};

    D1_VERIFY(::GetWindowPlacement(w, &wp));

    if (wp.showCmd == SW_MAXIMIZE or wp.showCmd == SW_MINIMIZE)
        return wp;

    const int s = ::GetSystemMetrics(SM_CYSIZE) + ::GetSystemMetrics(SM_CYFRAME);

    RECT monitor, work; // [virtual screen coordinates]
    getMonitorSizeAndWorkArea(w, monitor, work);


    RECT& n = wp.rcNormalPosition;
    D1_VERIFY(::OffsetRect(&n, s, s));

    // change workspace coordinates to screen coordinates
    D1_VERIFY(::OffsetRect(&n, work.left - monitor.left, work.top - monitor.top));

    if (n.right > work.right or n.bottom > work.bottom)
    {
        // make the top left corner of "n" equal to the top left corner of "work"
        D1_VERIFY(::OffsetRect(&n, work.left - n.left, work.top - n.top));
    }

    // change screen coordinates to workspace coordinates
    D1_VERIFY(::OffsetRect(&n, monitor.left - work.left, monitor.top - work.top));

    return wp;
}

}
