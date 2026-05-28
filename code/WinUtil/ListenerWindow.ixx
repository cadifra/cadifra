/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.ListenerWindow;

import d1.wintypes;

import WinUtil.ProcRegistrar;


namespace WinUtil
{

// The "Listener Window" is a hidden top level window that can be used
// to get diverse broadcasted system messages.
// The ListenerWindow class is an accessor for the shared "Listener Window".



export class ListenerWindow
{
    class Impl;
    static Impl& instance();

public:
    auto getDispatcher() const -> IDispatcher&; // (1)

    d1::HWND getWindowHandle() const; // (1)
};

}

// (1) Do not use the return value after the destruction of your
//     ListenerWindow instance.


module : private;

import WinUtil.Window;
import WinUtil.WindowClass;

import std;


namespace WinUtil
{

class ListenerWindow::Impl
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


ListenerWindow::Impl::Impl()
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


auto ListenerWindow::getDispatcher() const -> IDispatcher&
{
    return instance().getDispatcher();
}


HWND ListenerWindow::getWindowHandle() const
{
    return instance().getWindowHandle();
}


auto ListenerWindow::instance() -> Impl&
{
    static Impl singleton;
    return singleton;
}

}
