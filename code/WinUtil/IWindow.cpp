/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

module WinUtil.IWindow;

import d1.Rect;


namespace WinUtil
{

namespace
{
using C = IWindow;
}


bool C::getRect(d1::Rect& r) const
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


void C::updateWindow() const
{
    D1_VERIFY(::UpdateWindow(getWindowHandle()));
}


void C::setWindowTitle(const std::wstring& title)
{
    D1_VERIFY(::SetWindowText(getWindowHandle(), title.c_str()));
}

}
