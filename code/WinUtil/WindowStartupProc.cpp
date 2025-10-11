/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil;


namespace WinUtil
{

LRESULT CALLBACK WindowStartupProc(HWND h, UINT uMsg, WPARAM wp, LPARAM lp)
{
    if (uMsg == WM_CREATE && lp)
    {
        WinUtil::Window* window =
            reinterpret_cast<WinUtil::Window*>(
                reinterpret_cast<CREATESTRUCT*>(lp)->lpCreateParams);

        if (window)
        {
            window->SubClassWindow(h);
            window->SetDefaultWindowProc(&::DefWindowProc);
            return ::SendMessage(h, uMsg, wp, lp);
        }
    }

    return ::DefWindowProc(h, uMsg, wp, lp);
}

}
