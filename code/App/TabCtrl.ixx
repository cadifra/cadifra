/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <commctrl.h>

export module App.TabCtrl;

import d1.wintypes;


export namespace App::TabCtrl
{

// TabCtrl::IsDialogMessage is the same as ::IsDialogMessage but allows the user
// to switch between the tab pages with "CTRL+TAB", "CTRL+SHIFT+TAB", "CTRL+PG_UP"
// and "CTRL_+PG_DOWN".

bool isDialogMessage_(
    d1::HWND hDlg,  // handle to dialog box
    int tabCtrlId,  // id of the tab control
    d1::LPMSG lpMsg // message to be checked
);


d1::RECT getDisplayRect(d1::HWND t);
// Returns the display area of the tab control "t" in screen coordinates

}

module : private;

import WinUtil.Keyboard;


namespace App::TabCtrl
{

bool isDialogMessage_(
    HWND hDlg,     // handle to dialog box
    int tabCtrlId, // id of the tab control
    LPMSG lpMsg    // message to be checked
)
{
    if (not hDlg)
        return false;

    if (not lpMsg)
        return false;

    HWND tab = ::GetDlgItem(hDlg, tabCtrlId);

    if (not tab)
        return false;

    if (lpMsg->message == WM_KEYDOWN and
        (lpMsg->wParam == VK_TAB or
            lpMsg->wParam == VK_NEXT or
            lpMsg->wParam == VK_PRIOR) and
        WinUtil::Keyboard::controlIsDown() and
        ::IsChild(hDlg, lpMsg->hwnd))
    {
        int sel = TabCtrl_GetCurSel(tab);
        int count = TabCtrl_GetItemCount(tab);

        if ((lpMsg->wParam == VK_TAB and WinUtil::Keyboard::shiftIsDown()) or
            lpMsg->wParam == VK_PRIOR)
        {
            sel = (sel > 0) ? sel - 1 : count - 1;
        }
        else
        {
            if (++sel >= count)
                sel = 0;
        }

        auto nm = NMHDR{};
        nm.hwndFrom = tab;
        nm.idFrom = tabCtrlId;
        nm.code = TCN_SELCHANGING;

        if (not ::SendMessage(hDlg, WM_NOTIFY, tabCtrlId, reinterpret_cast<LPARAM>(&nm)))
        {
            TabCtrl_SetCurSel(tab, sel);

            nm.code = TCN_SELCHANGE;
            ::SendMessage(hDlg, WM_NOTIFY, tabCtrlId, reinterpret_cast<LPARAM>(&nm));
        }

        return true;
    }
    else
    {
        return 0 != ::IsDialogMessage(hDlg, lpMsg);
    }
}


RECT getDisplayRect(HWND tab)
{
    RECT r;
    D1_VERIFY(::GetWindowRect(tab, &r));
    TabCtrl_AdjustRect(tab, FALSE, &r);
    return r;
}

}
