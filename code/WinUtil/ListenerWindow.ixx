/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:ListenerWindow;

import d1.wintypes;

import :ProcRegistrar;


namespace WinUtil
{

// The "Listener Window" is a hidden top level window that can be used
// to get diverse broadcasted system messages.
// The ListenerWindow class is an accessor for the shared "Listener Window".



export class ListenerWindow
{
    class Impl;
    static Impl& Instance();

public:
    auto GetMsgDispatcher() const -> IWinMsgDispatcher&; // (1)

    d1::HWND GetWindowHandle() const; // (1)
};

}

// (1) Do not use the return value after the destruction of your
//     ListenerWindow instance.
