/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.Window;

import WinUtil.IWindow;


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
