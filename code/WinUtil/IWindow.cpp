/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

module WinUtil;

import d1.Rect;


namespace WinUtil
{

namespace
{
using C = IWindow;
}


bool C::GetRect(d1::Rect& r) const
{
    RECT wr;
    BOOL res = ::GetWindowRect(GetWindowHandle(), &wr);

    if (res != 0)
    {
        r = { wr.left, wr.top, wr.right, wr.bottom };
        return true;
    }

    return false;
}


void C::UpdateWindow() const
{
    D1_VERIFY(::UpdateWindow(GetWindowHandle()));
}


void C::SetWindowTitle(const std::wstring& title)
{
    D1_VERIFY(::SetWindowText(GetWindowHandle(), title.c_str()));
}

}
