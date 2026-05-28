/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module WinUtil.WindowPlacement;

import WinUtil.Registry;

import std;


namespace WinUtil
{

export class WindowPlacement: public WINDOWPLACEMENT
{
public:
    WindowPlacement();

    // uses compiler generated copy-ctor and assignment-op
};


export auto calcNewWindowPos(HWND w) -> WindowPlacement;

// Calculates a reasonable window position for a new top level window
// relative to an existing top level window "w".

// The new window position is maximized/minimized, if "w" is maximized/
// minimized.

// The new window is "cascaded" on the window "w" to the left downwards.
// If there is no more room on the workspace (workspace = desktop without
// toolbars)to "cascade" the new window, the top left position of the
// new window is in the top left corner of the workspace.

// The size of the new window is always the same as the size of "w".

// The new window is always on the same monitor as "w".


export class WindowPlacementHandler
{
    using ImpPtr = std::unique_ptr<WindowPlacement>;
    ImpPtr imp_;

public:
    WindowPlacementHandler() {}

    explicit WindowPlacementHandler(HWND w) { getFrom(w); }

    explicit WindowPlacementHandler(const WindowPlacement& wp):
        imp_{ std::make_unique<WindowPlacement>(wp) }
    {
    }

    WindowPlacementHandler(const WindowPlacementHandler&);
    WindowPlacementHandler& operator=(const WindowPlacementHandler&);

    bool ok() const { return imp_.get() != nullptr; }

    void changeShowCmd(UINT flags);

    void getFrom(HWND w);
    void setTo(HWND w) const;

    // Read and Write may throw a Registry::Exception
    void read(const Registry::Key& k, const std::wstring& valueName);
    void write(const Registry::Key& k, const std::wstring& valueName) const;
};

}


module :private;

namespace WinUtil
{

WindowPlacement::WindowPlacement():
    WINDOWPLACEMENT{}
{
    length = sizeof(WINDOWPLACEMENT);
}

}


namespace WinUtil
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


namespace WinUtil
{

WindowPlacementHandler::WindowPlacementHandler(
    const WindowPlacementHandler& rhs):

    imp_{ rhs.imp_.get()
              ? std::make_unique<WindowPlacement>(*rhs.imp_)
              : nullptr }
{
}


auto WindowPlacementHandler::operator=(
    const WindowPlacementHandler& rhs) -> WindowPlacementHandler&
{
    if (this == &rhs)
        return *this;

    imp_ = rhs.imp_.get()
               ? std::make_unique<WindowPlacement>(*rhs.imp_)
               : nullptr;

    return *this;
}


void WindowPlacementHandler::getFrom(HWND w)
{
    imp_.reset();
    auto t = std::make_unique<WindowPlacement>();

    if (not ::GetWindowPlacement(w, t.get()))
        return;

    imp_ = std::move(t);
}


void WindowPlacementHandler::setTo(HWND w) const
{
    if (not ok())
        return;

    const BOOL res = ::SetWindowPlacement(w, imp_.get());
    D1_ASSERT(res);
}


void WindowPlacementHandler::read(const Registry::Key& k, const std::wstring& vn)
{
    imp_.reset();
    auto t = std::make_unique<WindowPlacement>();

    using WP = WINDOWPLACEMENT;
    WP* wp = t.get();

    if (not Registry::query(k, vn, reinterpret_cast<BYTE*>(wp), sizeof(WP)))
        return;

    if (t->length != sizeof(WP))
        return;

    imp_ = std::move(t);
}


void WindowPlacementHandler::write(
    const Registry::Key& k, const std::wstring& vn) const
{
    if (not ok())
        return;

    using WP = WINDOWPLACEMENT;
    WP* wp = imp_.get();

    Registry::set(k, vn, reinterpret_cast<const BYTE*>(wp), sizeof(WP));
}


void WindowPlacementHandler::changeShowCmd(UINT showCmd)
{
    if (not ok())
        return;
    imp_->showCmd = showCmd;
}

}
