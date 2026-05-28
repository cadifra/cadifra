/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module WinUtil.WindowResizer;

import WinUtil.IWindow;
import WinUtil.Messages;
import WinUtil.ProcRegistrar;

import d1.wintypes;


namespace WinUtil
{

export class WindowResizer
{
public:
    WindowResizer(IWindow& theWindow);

    WindowResizer(const WindowResizer&) = delete;
    WindowResizer& operator=(const WindowResizer&) = delete;

    class ResizeAlgorithm;

    // Standard algorithms
    struct Left;
    struct Top;
    struct Right;
    struct Bottom;

    template <class H, class V>
    class RelativeMover;

    template <class L, class T, class R, class B>
    class Stretcher;

    void registerChild(ResizeAlgorithm& ra)
    {
        algorithms_.push_back(&ra);
    }

    void storePos();
    void newSize();

private:
    void onSettingChange(WM_SETTINGCHANGE_Msg)
    {
        storePos();
    }

    void onWinPosChanged(WM_WINDOWPOSCHANGED_Msg);

    using This = WindowResizer;
    IWindow& window_;
    ProcRegistrar procReg_;

    using Vector = std::vector<ResizeAlgorithm*>;
    Vector algorithms_;
    RECT basePos_ = {};
};


class WindowResizer::ResizeAlgorithm
{
public:
    virtual HDWP callDeferWindowPos(HDWP dwp,
        const RECT& oldParent, const RECT& newParent) = 0;

    virtual int storePos() = 0;

protected:
    ~ResizeAlgorithm() = default;
};


struct WindowResizer::Left
{
    static LONG x(const RECT& r) { return r.left; }
};

struct WindowResizer::Top
{
    static LONG y(const RECT& r) { return r.top; }
};

struct WindowResizer::Right
{
    static LONG x(const RECT& r) { return r.right; }
};

struct WindowResizer::Bottom
{
    static LONG y(const RECT& r) { return r.bottom; }
};


template <class H = Left, class V = Top>
class WindowResizer::RelativeMover: public ResizeAlgorithm
{
    d1::HWND child_;
    RECT oldPos_;

public:
    RelativeMover(d1::HWND child):
        child_{ child }
    {
        storePos();
    }

    HDWP callDeferWindowPos(HDWP hdwp,
        const RECT& oldParent, const RECT& newParent)
    {
        RECT r = oldPos_;

        OffsetRect(&r,
            H::x(newParent) - H::x(oldParent),
            V::y(newParent) - V::y(oldParent));

        MapWindowPoints(0, GetParent(child_), reinterpret_cast<POINT*>(&r), 2);

        return DeferWindowPos(hdwp, child_, 0, r.left, r.top, 0, 0,
            SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }

    int storePos()
    {
        return GetWindowRect(child_, &oldPos_);
    }
};


template <class L = Left, class T = Top, class R = Right, class B = Bottom>
class WindowResizer::Stretcher: public ResizeAlgorithm
{
    d1::HWND child_;
    RECT oldPos_;

public:
    Stretcher(d1::HWND child):
        child_{ child }
    {
        storePos();
    }

    HDWP callDeferWindowPos(HDWP hdwp,
        const RECT& oldParent, const RECT& newParent)
    {
        auto rc = RECT{ oldPos_ };

        rc.left += L::x(newParent) - L::x(oldParent);
        rc.top += T::y(newParent) - T::y(oldParent);
        rc.right += R::x(newParent) - R::x(oldParent);
        rc.bottom += B::y(newParent) - B::y(oldParent);

        MapWindowPoints(0, GetParent(child_), reinterpret_cast<POINT*>(&rc), 2);

        return DeferWindowPos(hdwp, child_, 0,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOZORDER);
    }

    int storePos()
    {
        return GetWindowRect(child_, &oldPos_);
    }
};

}


module :private;


namespace WinUtil
{

inline int mapWindowRect(HWND hWndFrom, HWND hWndTo, RECT& r)
{
    return MapWindowPoints(hWndFrom, hWndTo, reinterpret_cast<LPPOINT>(&r), 2);
}


WindowResizer::WindowResizer(IWindow& theWindow):

    window_{ theWindow },
    procReg_{ theWindow.getDispatcher(), 0 }
{
    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onSettingChange);
    ph.addSpy(&This::onWinPosChanged);

    D1_VERIFY(::GetClientRect(window_.getWindowHandle(), &basePos_));
    mapWindowRect(window_.getWindowHandle(), 0, basePos_);
}


void WindowResizer::newSize()
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


void WindowResizer::storePos()
{
    D1_VERIFY(::GetClientRect(window_.getWindowHandle(), &basePos_));
    mapWindowRect(window_.getWindowHandle(), 0, basePos_);

    for (auto ra : algorithms_)
        ra->storePos();
}


void WindowResizer::onWinPosChanged(WM_WINDOWPOSCHANGED_Msg msg)
{
    LPWINDOWPOS wp = msg.wpos();

    if (wp->flags & SWP_NOSIZE)
        return;

    newSize();
}

}
