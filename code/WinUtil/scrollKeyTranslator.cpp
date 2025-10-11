/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil;


namespace WinUtil
{

namespace
{

void Send(HWND receiver, UINT msg, WORD request, HWND sender)
{
    const LPARAM lparam = reinterpret_cast<LPARAM>(sender);

    ::SendMessage(receiver, msg, MAKELONG(request, 0), lparam);
    ::SendMessage(receiver, msg, MAKELONG(SB_ENDSCROLL, 0), lparam);
}

}


void scrollKeyTranslator(
    HWND receiver,
    HWND hScrollBar,
    HWND vScrollBar,
    const WM_KEYDOWN_Msg& msg)
{
    switch (msg.nVirtKey())
    {
    case VK_LEFT:
        if (::GetKeyState(VK_CONTROL) > 1U)
            Send(receiver, WM_HSCROLL, SB_TOP, hScrollBar);
        else
            Send(receiver, WM_HSCROLL, SB_LINEUP, hScrollBar);
        break;

    case VK_UP:
        if (::GetKeyState(VK_CONTROL) > 1U)
            Send(receiver, WM_VSCROLL, SB_TOP, vScrollBar);
        else
            Send(receiver, WM_VSCROLL, SB_LINEUP, vScrollBar);
        break;

    case VK_PRIOR:
        if (::GetKeyState(VK_SHIFT) > 1U)
            Send(receiver, WM_HSCROLL, SB_PAGEUP, hScrollBar);
        else
            Send(receiver, WM_VSCROLL, SB_PAGEUP, vScrollBar);
        break;

    case VK_NEXT:
        if (::GetKeyState(VK_SHIFT) > 1U)
            Send(receiver, WM_HSCROLL, SB_PAGEDOWN, hScrollBar);
        else
            Send(receiver, WM_VSCROLL, SB_PAGEDOWN, vScrollBar);
        break;

    case VK_DOWN:
        if (::GetKeyState(VK_CONTROL) > 1U)
            Send(receiver, WM_VSCROLL, SB_BOTTOM, vScrollBar);
        else
            Send(receiver, WM_VSCROLL, SB_LINEDOWN, vScrollBar);
        break;

    case VK_RIGHT:
        if (::GetKeyState(VK_CONTROL) > 1U)
            Send(receiver, WM_HSCROLL, SB_BOTTOM, hScrollBar);
        else
            Send(receiver, WM_HSCROLL, SB_LINEDOWN, hScrollBar);
        break;

    case VK_HOME:
        Send(receiver, WM_VSCROLL, SB_TOP, vScrollBar);
        Send(receiver, WM_HSCROLL, SB_TOP, hScrollBar);
        break;

    case VK_END:
        Send(receiver, WM_VSCROLL, SB_BOTTOM, vScrollBar);
        Send(receiver, WM_HSCROLL, SB_BOTTOM, hScrollBar);
        break;
    }
}

}
