/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

module WinUtil;


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

    itsInstance{ hInstance }
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

    itsAtom = ::RegisterClassEx(&wc);
    D1_ASSERT(itsAtom);
}


C::~WindowClass()
{
    D1_VERIFY(::UnregisterClass(MAKEINTATOM(itsAtom), itsInstance));
}

}
