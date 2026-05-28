/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.SuspendQuit;


namespace WinUtil
{

export class SuspendQuit
// Temporarily removes in its ctor a WM_QUIT message from the
// message loop and reposts it in its dtor.
{
public:
    SuspendQuit();

    SuspendQuit(const SuspendQuit&) = delete;
    SuspendQuit& operator=(const SuspendQuit&) = delete;

    ~SuspendQuit(); // intentionally not virtual

private:
    bool removedQuitMessage_ = false;
    int exitCode_ = -1;
};


SuspendQuit::SuspendQuit()
{
    auto msg = MSG{};
    removedQuitMessage_ =
        0 != ::PeekMessage(&msg, 0, WM_QUIT, WM_QUIT, PM_REMOVE);

    if (removedQuitMessage_ and msg.message == WM_QUIT)
        exitCode_ = static_cast<int>(msg.wParam);
}


SuspendQuit::~SuspendQuit()
{
    if (removedQuitMessage_)
        ::PostQuitMessage(exitCode_);
}

}
