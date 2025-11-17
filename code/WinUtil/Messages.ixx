/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "MessageWrapperMacros.h"

export module WinUtil.Messages;

export import WinUtil.Message;

import d1.MouseButton;
import d1.Point;


export namespace WinUtil
{
// App

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_ACTIVATEAPP_Msg, WM_ACTIVATEAPP)
BOOL fActive() const { return static_cast<BOOL>(GetWParam()); }
DWORD dwThreadID() const { return static_cast<DWORD>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_SETTINGCHANGE_Msg, WM_SETTINGCHANGE)
UINT wFlag() const { return static_cast<UINT>(GetWParam()); }
LPCTSTR pszSection() const { return reinterpret_cast<LPCTSTR>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_NO_PARAM(WM_SYSCOLORCHANGE_Msg, WM_SYSCOLORCHANGE)

D1_WINMESSAGE_WRAPPER_CLASS_NO_PARAM(WM_FONTCHANGE_Msg, WM_FONTCHANGE)


// Clipboard

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_CHANGECBCHAIN_Msg, WM_CHANGECBCHAIN)
HWND hwndRemove() const { return reinterpret_cast<HWND>(GetWParam()); }
HWND hwndNext() const { return reinterpret_cast<HWND>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_NO_PARAM(WM_DRAWCLIPBOARD_Msg, WM_DRAWCLIPBOARD)

D1_WINMESSAGE_WRAPPER_CLASS_NO_PARAM(WM_CLIPBOARDUPDATE_Msg, WM_CLIPBOARDUPDATE)


// Command

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_COMMAND_Msg, WM_COMMAND)
WORD wNotifyCode() const { return HIWORD(GetWParam()); }
WORD wID() const { return LOWORD(GetWParam()); }
HWND hwndCtl() const { return reinterpret_cast<HWND>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


// CopyData

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_COPYDATA_Msg, WM_COPYDATA)
PCOPYDATASTRUCT copyData() const
{
    return reinterpret_cast<PCOPYDATASTRUCT>(GetLParam());
}
D1_WINMESSAGE_WRAPPER_CLASS_END


// DeviceContext

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_PAINT_Msg, WM_PAINT)
HDC hdc() const { return reinterpret_cast<HDC>(GetWParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_ERASEBKGND_Msg, WM_ERASEBKGND)
HDC hdc() const { return reinterpret_cast<HDC>(GetWParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


// Dialog

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_INITDIALOG_Msg, WM_INITDIALOG)
HWND defaultControl() const { return reinterpret_cast<HWND>(GetWParam()); }
LPARAM initData() const { return GetLParam(); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_DRAWITEM_Msg, WM_DRAWITEM)
UINT controlId() const { return static_cast<UINT>(GetWParam()); }
LPDRAWITEMSTRUCT itemData() const { return reinterpret_cast<LPDRAWITEMSTRUCT>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_GETDLGCODE_Msg, WM_GETDLGCODE)
LPMSG messageInfo() const { return reinterpret_cast<LPMSG>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_SETFONT_Msg, WM_SETFONT)
HFONT font() const { return reinterpret_cast<HFONT>(GetWParam()); }
BOOL redraw() const { return static_cast<BOOL>(LOWORD(GetLParam())); }
D1_WINMESSAGE_WRAPPER_CLASS_END


// Keyboard

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_KEYDOWN_Msg, WM_KEYDOWN)
int nVirtKey() const { return static_cast<int>(GetWParam()); }
LPARAM lKeyData() const { return GetLParam(); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_KEYUP_Msg, WM_KEYUP)
int nVirtKey() const { return static_cast<int>(GetWParam()); }
LPARAM lKeyData() const { return GetLParam(); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_CHAR_Msg, WM_CHAR)
TCHAR chCharCode() const { return static_cast<TCHAR>(GetWParam()); }
LPARAM lKeyData() const { return GetLParam(); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_SETFOCUS_Msg, WM_SETFOCUS)
HWND hwndLoseFocus() const { return reinterpret_cast<HWND>(GetWParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_KILLFOCUS_Msg, WM_KILLFOCUS)
HWND hwndGetFocus() const { return reinterpret_cast<HWND>(GetWParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


// Menu

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_INITMENU_Msg, WM_INITMENU)
HMENU hmenuInit() const { return reinterpret_cast<HMENU>(GetWParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_INITMENUPOPUP_Msg, WM_INITMENUPOPUP)
HMENU hmenuPopup() const { return reinterpret_cast<HMENU>(GetWParam()); }
UINT uPos() const { return static_cast<UINT>(LOWORD(GetLParam())); }
BOOL fSystemMenu() const { return static_cast<BOOL>(HIWORD(GetLParam())); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_CONTEXTMENU_Msg, WM_CONTEXTMENU)
HWND hwnd() const { return reinterpret_cast<HWND>(GetWParam()); }
short xPos() const { return static_cast<short>(LOWORD(GetLParam())); }
short yPos() const { return static_cast<short>(HIWORD(GetLParam())); }
d1::Point GetClientCoordinates() const;
bool IsKeyboardGenerated() const;
D1_WINMESSAGE_WRAPPER_CLASS_END


// Notification

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_NOTIFY_Msg, WM_NOTIFY)
int idCtrl() const { return static_cast<int>(GetWParam()); }
LPNMHDR pnmh() const { return reinterpret_cast<LPNMHDR>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


// Scroll

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_VSCROLL_Msg, WM_VSCROLL)
int nScrollCode() const { return static_cast<int>(LOWORD(GetWParam())); }
short int nPos() const { return static_cast<short int>(HIWORD(GetWParam())); }
HWND hwndScrollBar() const { return reinterpret_cast<HWND>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_HSCROLL_Msg, WM_HSCROLL)
int nScrollCode() const { return static_cast<int>(LOWORD(GetWParam())); }
short int nPos() const { return static_cast<short int>(HIWORD(GetWParam())); }
HWND hwndScrollBar() const { return reinterpret_cast<HWND>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


// Session

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_QUERYENDSESSION_Msg, WM_QUERYENDSESSION)
UINT nSource() const { return static_cast<UINT>(GetWParam()); }
LPARAM fLogOff() const { return GetLParam(); }
D1_WINMESSAGE_WRAPPER_CLASS_END


// Timer

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_TIMER_Msg, WM_TIMER)
UINT wTimerID() const { return static_cast<UINT>(GetWParam()); }
TIMERPROC* tmprc() const { return reinterpret_cast<TIMERPROC*>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


// Windows

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_SIZE_Msg, WM_SIZE)
WPARAM fwSizeType() const { return GetWParam(); }
WORD nWidth() const { return LOWORD(GetLParam()); }
WORD nHeight() const { return HIWORD(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END

D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_SIZING_Msg, WM_SIZING)
WPARAM what() const { return GetWParam(); }
LPRECT rect() const { return reinterpret_cast<LPRECT>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_WINDOWPOSCHANGED_Msg, WM_WINDOWPOSCHANGED)
LPWINDOWPOS wpos() const { return reinterpret_cast<LPWINDOWPOS>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_MDIACTIVATE_Msg, WM_MDIACTIVATE)
HWND hwndChildDeact() const { return reinterpret_cast<HWND>(GetWParam()); }
HWND hwndChildAct() const { return reinterpret_cast<HWND>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_ACTIVATE_Msg, WM_ACTIVATE)
UINT state() const { return static_cast<UINT>(LOWORD(GetWParam())); }
HWND hwndActDeact() const { return reinterpret_cast<HWND>(GetLParam()); }
BOOL fMinimized() const { return static_cast<BOOL>(HIWORD(GetWParam())); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_CREATE_Msg, WM_CREATE)
LPCREATESTRUCT lpcs() const { return reinterpret_cast<LPCREATESTRUCT>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


D1_WINMESSAGE_WRAPPER_CLASS_NO_PARAM(WM_DESTROY_Msg, WM_DESTROY)

D1_WINMESSAGE_WRAPPER_CLASS_NO_PARAM(WM_CLOSE_Msg, WM_CLOSE)


D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_NCACTIVATE_Msg, WM_NCACTIVATE)
BOOL fActive() const { return static_cast<BOOL>(GetWParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END


//
// Mouse button messages:
//
template <UINT MsgId>
class WM_XBUTTONX_Msg:
    public StaticWinMsgWrapper<MsgId>
{
public:
    WM_XBUTTONX_Msg(Message& msg):
        StaticWinMsgWrapper<MsgId>(msg) {}

    WPARAM fwKeys() const { return this->GetWParam(); }
    short xPos() const { return static_cast<short>(LOWORD(this->GetLParam())); }
    short yPos() const { return static_cast<short>(HIWORD(this->GetLParam())); }
};


D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(
    WM_LBUTTONDOWN_Msg, WM_LBUTTONDOWN, WM_XBUTTONX_Msg)

D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(
    WM_LBUTTONUP_Msg, WM_LBUTTONUP, WM_XBUTTONX_Msg)

D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(
    WM_LBUTTONDBLCLK_Msg, WM_LBUTTONDBLCLK, WM_XBUTTONX_Msg)


D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(
    WM_RBUTTONDOWN_Msg, WM_RBUTTONDOWN, WM_XBUTTONX_Msg)

D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(
    WM_RBUTTONUP_Msg, WM_RBUTTONUP, WM_XBUTTONX_Msg)

D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(
    WM_RBUTTONDBLCLK_Msg, WM_RBUTTONDBLCLK, WM_XBUTTONX_Msg)


D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(
    WM_MBUTTONDOWN_Msg, WM_MBUTTONDOWN, WM_XBUTTONX_Msg)

D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(
    WM_MBUTTONUP_Msg, WM_MBUTTONUP, WM_XBUTTONX_Msg)

D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(
    WM_MBUTTONDBLCLK_Msg, WM_MBUTTONDBLCLK, WM_XBUTTONX_Msg)




D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_MOUSEMOVE_Msg, WM_MOUSEMOVE)
WPARAM fwKeys() const { return GetWParam(); }
short xPos() const { return static_cast<short>(LOWORD(GetLParam())); }
short yPos() const { return static_cast<short>(HIWORD(GetLParam())); }
bool ButtonDown(d1::MouseButton) const;
D1_WINMESSAGE_WRAPPER_CLASS_END



D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_MOUSEWHEEL_Msg, WM_MOUSEWHEEL)
WORD fwKeys() const { return LOWORD(GetWParam()); }
short zDelta() const { return static_cast<short>(HIWORD(GetWParam())); }
short xPos() const { return static_cast<short>(LOWORD(GetLParam())); }
short yPos() const { return static_cast<short>(HIWORD(GetLParam())); }
D1_WINMESSAGE_WRAPPER_CLASS_END



D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_MOUSEACTIVATE_Msg, WM_MOUSEACTIVATE)
HWND hwndTopLevel() const { return reinterpret_cast<HWND>(GetWParam()); }
INT nHittest() const { return static_cast<INT>(LOWORD(GetLParam())); }
UINT uMsg() const { return static_cast<UINT>(HIWORD(GetLParam())); }
D1_WINMESSAGE_WRAPPER_CLASS_END



D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_SETCURSOR_Msg, WM_SETCURSOR)
HWND hwnd() const { return reinterpret_cast<HWND>(GetWParam()); }
INT nHittest() const { return static_cast<INT>(LOWORD(GetLParam())); }
UINT wMouseMsg() const { return static_cast<UINT>(HIWORD(GetLParam())); }
D1_WINMESSAGE_WRAPPER_CLASS_END



D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(WM_CAPTURECHANGED_Msg, WM_CAPTURECHANGED)
HWND hwnd() const { return reinterpret_cast<HWND>(GetLParam()); }
D1_WINMESSAGE_WRAPPER_CLASS_END

}
