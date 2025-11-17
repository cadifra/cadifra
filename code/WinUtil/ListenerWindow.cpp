/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.ListenerWindow;

import WinUtil.IWindow;
import WinUtil.Window;

import std;


namespace WinUtil
{

namespace
{
using C = ListenerWindow;
}

class C::Impl
{
public:
    Impl();

    HWND GetWindowHandle() const { return itsWindow.GetWindowHandle(); }

    auto GetDispatcher() -> IDispatcher&
    {
        return itsWindow.GetDispatcher();
    }

private:
    Window itsWindow;
};


C::Impl::Impl()
{
    HINSTANCE i = ::GetModuleHandle(0);

    static auto wc = WinUtil::WindowClass{
        TEXT("Cadifra Listener"),    // lpszClassName
        &WinUtil::WindowStartupProc, // lpfnWndProc
        i                            // hInstance
    };

    ::CreateWindow(
        wc.GetAtom(), // lpClassName
        0,            // lpWindowName
        WS_DISABLED,  // dwStyle
        0, 0,         // x, y
        0, 0,         // nWidth, nHeight
        0,            // hWndParent
        0,            // hMenu
        i,            // hInstance
        &itsWindow    // lpParam
    );
}


auto C::GetDispatcher() const -> IDispatcher&
{
    return Instance().GetDispatcher();
}


HWND C::GetWindowHandle() const
{
    return Instance().GetWindowHandle();
}


auto C::Instance() -> Impl&
{
    static Impl singleton;
    return singleton;
}

}
