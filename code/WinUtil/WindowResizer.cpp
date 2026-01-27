/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

module WinUtil.IWindow;


namespace WinUtil
{

namespace
{
inline int mapWindowRect(HWND hWndFrom, HWND hWndTo, RECT& r)
{
    return MapWindowPoints(hWndFrom, hWndTo, reinterpret_cast<LPPOINT>(&r), 2);
}

using C = WindowResizer;
}


C::WindowResizer(IWindow& theWindow):
    window_{ theWindow },
    procReg_{ theWindow.getDispatcher(), 0 }
{
    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onSettingChange);
    ph.addSpy(&This::onWinPosChanged);

    D1_VERIFY(::GetClientRect(window_.getWindowHandle(), &basePos_));
    mapWindowRect(window_.getWindowHandle(), 0, basePos_);
}


void C::registerChild(ResizeAlgorithm& ra)
{
    algorithms_.push_back(&ra);
}


void C::newSize()
{
    if (::IsIconic(window_.getWindowHandle()))
        return;

    RECT newParentRect;
    D1_VERIFY(::GetClientRect(window_.getWindowHandle(), &newParentRect));
    mapWindowRect(window_.getWindowHandle(), 0, newParentRect);

    HDWP hdwp = ::BeginDeferWindowPos(static_cast<int>(algorithms_.size()));
    D1_ASSERT(hdwp);

    for (auto* algo : algorithms_)
    {
        if (not hdwp)
            break;
        hdwp = algo->callDeferWindowPos(hdwp, basePos_, newParentRect);
    }

    if (hdwp)
        ::EndDeferWindowPos(hdwp);
}


void C::storePos()
{
    D1_VERIFY(::GetClientRect(window_.getWindowHandle(), &basePos_));
    mapWindowRect(window_.getWindowHandle(), 0, basePos_);

    for (auto ra : algorithms_)
        ra->storePos();
}


void C::onSettingChange(WM_SETTINGCHANGE_Msg)
{
    storePos();
}


void C::onWinPosChanged(WM_WINDOWPOSCHANGED_Msg msg)
{
    LPWINDOWPOS wp = msg.wpos();

    if (wp->flags & SWP_NOSIZE)
        return;

    newSize();
}

}
