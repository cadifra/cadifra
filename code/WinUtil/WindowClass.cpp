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
using C = WindowClass;
}


C::WindowClass(
    LPCTSTR lpszClassName,
    WNDPROC lpfnWndProc,
    HINSTANCE hInstance,
    UINT style,
    HBRUSH hbrBackground,
    HCURSOR hCursor,
    LPCTSTR lpszMenuName,
    HICON hIcon,
    HICON hIconSm,
    int cbClsExtra,
    int cbWndExtra):

    instance_{ hInstance }
{
    auto wc = WNDCLASSEX{};

    wc.cbSize = sizeof(wc);
    wc.style = style;
    wc.lpfnWndProc = lpfnWndProc;
    wc.cbClsExtra = cbClsExtra;
    wc.cbWndExtra = cbWndExtra;
    wc.hInstance = hInstance;
    wc.hIcon = hIcon;
    wc.hCursor = hCursor;
    wc.hbrBackground = hbrBackground;
    wc.lpszMenuName = lpszMenuName;
    wc.lpszClassName = lpszClassName;
    wc.hIconSm = hIconSm;

    atom_ = ::RegisterClassEx(&wc);
    D1_ASSERT(atom_);
}


C::~WindowClass()
{
    D1_VERIFY(::UnregisterClass(MAKEINTATOM(atom_), instance_));
}

}
