/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module WinUtil.IWindow;

import WinUtil.ProcRegistrar;

import d1.Rect;
import d1.wintypes;

import std;


namespace WinUtil
{

export class IWindow
{
public:
    virtual d1::HWND getWindowHandle() const = 0;

    virtual auto getDispatcher() const -> IDispatcher& = 0;

    bool getRect(d1::Rect& r) const;
    // Writes the window rect in screen coordinates to r and returns true.
    // Returns false if the call fails

    void updateWindow() const;
    void setWindowTitle(const std::wstring& title);

protected:
    ~IWindow() = default;
};


bool IWindow::getRect(d1::Rect& r) const
{
    RECT wr;
    BOOL res = ::GetWindowRect(getWindowHandle(), &wr);

    if (res != 0)
    {
        r = { wr.left, wr.top, wr.right, wr.bottom };
        return true;
    }

    return false;
}


void IWindow::updateWindow() const
{
    D1_VERIFY(::UpdateWindow(getWindowHandle()));
}


void IWindow::setWindowTitle(const std::wstring& title)
{
    D1_VERIFY(::SetWindowText(getWindowHandle(), title.c_str()));
}

}
