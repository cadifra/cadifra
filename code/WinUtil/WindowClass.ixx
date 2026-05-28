/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module WinUtil.WindowClass;


namespace WinUtil
{

export class WindowClass
{
public:
    WindowClass(
        LPCTSTR lpszClassName,
        WNDPROC lpfnWndProc,
        HINSTANCE hInstance,
        UINT style = 0,
        HBRUSH hbrBackground = 0,
        HCURSOR hCursor = 0,
        LPCTSTR lpszMenuName = 0,
        HICON hIcon = 0,
        HICON hIconSm = 0,
        int cbClsExtra = 0,
        int cbWndExtra = 0);
    // calls RegisterClassEx

    WindowClass(const WindowClass&) = delete;
    WindowClass& operator=(const WindowClass&) = delete;

    ~WindowClass(); // intentionally not virtual
    // calls UnregisterClass


    LPCTSTR getAtom() const { return MAKEINTATOM(atom_); }

private:
    ATOM atom_ = {};
    HINSTANCE instance_ = {};
};


export LRESULT CALLBACK windowStartupProc(HWND h, UINT uMsg, WPARAM wp, LPARAM lp);

// Use this function as the WNDPROC for windows you create with "CreateWindow"
// or "CreateWindowEx" with the "lpParam" set to a WinUtil::Window pointer.

// The WindowStartupProc will wait for the WM_CREATE message, extract the pointer
// to the WinUtil::Window and subclass the window.

}


module : private;


namespace WinUtil
{

WindowClass::WindowClass(
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


WindowClass::~WindowClass()
{
    D1_VERIFY(::UnregisterClass(MAKEINTATOM(atom_), instance_));
}

}


import WinUtil.Window;


namespace WinUtil
{

LRESULT CALLBACK windowStartupProc(HWND h, UINT uMsg, WPARAM wp, LPARAM lp)
{
    if (uMsg == WM_CREATE and lp)
    {
        WinUtil::Window* window =
            reinterpret_cast<WinUtil::Window*>(
                reinterpret_cast<CREATESTRUCT*>(lp)->lpCreateParams);

        if (window)
        {
            window->subClassWindow(h);
            window->setDefaultWindowProc(&::DefWindowProc);
            return ::SendMessage(h, uMsg, wp, lp);
        }
    }

    return ::DefWindowProc(h, uMsg, wp, lp);
}

}
