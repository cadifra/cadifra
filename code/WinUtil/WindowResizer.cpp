/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

module WinUtil;


namespace WinUtil
{

namespace
{
inline int MapWindowRect(HWND hWndFrom, HWND hWndTo, RECT& r)
{
    return MapWindowPoints(hWndFrom, hWndTo, reinterpret_cast<LPPOINT>(&r), 2);
}

using C = WindowResizer;
}


C::WindowResizer(IWindow& theWindow):
    itsWindow{ theWindow },
    itsProcReg{ theWindow.GetMsgDispatcher(), 0 }
{
    auto ph = itsProcReg.Helper(*this);

    ph.RegisterSpy(&This::OnSettingChange);
    ph.RegisterSpy(&This::OnWinPosChanged);

    D1_VERIFY(::GetClientRect(itsWindow.GetWindowHandle(), &itsBasePos));
    MapWindowRect(itsWindow.GetWindowHandle(), 0, itsBasePos);
}


void C::RegisterChild(ResizeAlgorithm& ra)
{
    itsAlgorithms.push_back(&ra);
}


void C::NewSize()
{
    if (::IsIconic(itsWindow.GetWindowHandle()))
        return;

    RECT newParentRect;
    D1_VERIFY(::GetClientRect(itsWindow.GetWindowHandle(), &newParentRect));
    MapWindowRect(itsWindow.GetWindowHandle(), 0, newParentRect);

    HDWP hdwp = ::BeginDeferWindowPos(static_cast<int>(itsAlgorithms.size()));
    D1_ASSERT(hdwp);

    for (auto* algo : itsAlgorithms)
    {
        if (!hdwp)
            break;
        hdwp = algo->CallDeferWindowPos(hdwp, itsBasePos, newParentRect);
    }

    if (hdwp)
        ::EndDeferWindowPos(hdwp);
}


void C::StorePos()
{
    D1_VERIFY(::GetClientRect(itsWindow.GetWindowHandle(), &itsBasePos));
    MapWindowRect(itsWindow.GetWindowHandle(), 0, itsBasePos);

    for (auto ra : itsAlgorithms)
        ra->StorePos();
}


void C::OnSettingChange(WM_SETTINGCHANGE_Msg)
{
    StorePos();
}


void C::OnWinPosChanged(WM_WINDOWPOSCHANGED_Msg msg)
{
    LPWINDOWPOS wp = msg.wpos();

    if (wp->flags & SWP_NOSIZE)
        return;

    NewSize();
}

}
