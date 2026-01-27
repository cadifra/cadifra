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

    HWND getWindowHandle() const { return window_.getWindowHandle(); }

    auto getDispatcher() -> IDispatcher&
    {
        return window_.getDispatcher();
    }

private:
    Window window_;
};


C::Impl::Impl()
{
    HINSTANCE i = ::GetModuleHandle(0);

    static auto wc = WinUtil::WindowClass{
        TEXT("Cadifra Listener"),    // lpszClassName
        &WinUtil::windowStartupProc, // lpfnWndProc
        i                            // hInstance
    };

    ::CreateWindow(
        wc.getAtom(), // lpClassName
        0,            // lpWindowName
        WS_DISABLED,  // dwStyle
        0, 0,         // x, y
        0, 0,         // nWidth, nHeight
        0,            // hWndParent
        0,            // hMenu
        i,            // hInstance
        &window_    // lpParam
    );
}


auto C::getDispatcher() const -> IDispatcher&
{
    return instance().getDispatcher();
}


HWND C::getWindowHandle() const
{
    return instance().getWindowHandle();
}


auto C::instance() -> Impl&
{
    static Impl singleton;
    return singleton;
}

}
