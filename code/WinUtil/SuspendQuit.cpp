/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.MessageLoop;


namespace WinUtil
{

namespace
{
using C = SuspendQuit;
}


C::SuspendQuit()
{
    auto msg = MSG{};
    itRemovedQuitMessage =
        0 != ::PeekMessage(&msg, 0, WM_QUIT, WM_QUIT, PM_REMOVE);

    if (itRemovedQuitMessage && msg.message == WM_QUIT)
        itsExitCode = static_cast<int>(msg.wParam);
}


C::~SuspendQuit()
{
    if (itRemovedQuitMessage)
        ::PostQuitMessage(itsExitCode);
}

}
