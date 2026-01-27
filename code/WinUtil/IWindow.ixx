/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.IWindow;

import WinUtil.Messages;
import WinUtil.ProcRegistrar;

import d1.Rect;
import d1.wintypes;

import std;


namespace WinUtil
{

export class IWindow
{
public:
    virtual d1::HWND getWindowHandle() const = 0;

    virtual auto getDispatcher() const -> IDispatcher& = 0;

    bool getRect(d1::Rect& r) const;
    // Writes the window rect in screen coordinates to r and returns true.
    // Returns false if the call fails

    void updateWindow() const;
    void setWindowTitle(const std::wstring& title);

protected:
    ~IWindow() = default;
};


export template <typename T>
class WindowHandleTable
{
    using Entry = std::pair<d1::HWND, T*>;
    std::vector<Entry> table_;

    void remove(d1::HWND h)
    {
        auto it = std::ranges::find(table_, h, &Entry::first);
        if (it != end(table_))
            table_.erase(it);
    }

public:
    class DoRemove;
    using Remover = std::unique_ptr<DoRemove>;

    auto add(d1::HWND h, T* p) -> Remover
    {
        table_.push_back({ h, p });
        return std::make_unique<DoRemove>(this, h);
    }

    T* find(d1::HWND h)
    {
        auto it = std::ranges::find(table_, h, &Entry::first);
        return (it != end(table_)) ? it->second : nullptr;
    }

    WindowHandleTable() = default;
    WindowHandleTable(const WindowHandleTable&) = delete;
    WindowHandleTable& operator=(const WindowHandleTable&) = delete;
};


export template <typename T>
class WindowHandleTable<T>::DoRemove
{
    WindowHandleTable* WHT_;
    d1::HWND hWND_;

public:
    DoRemove(WindowHandleTable* t, d1::HWND w):
        WHT_{ t }, hWND_{ w }
    {
    }
    ~DoRemove() { WHT_->remove(hWND_); }
};


export class Window: public IWindow
{
public:
    Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    virtual ~Window();

    //-- IWindow

    auto getDispatcher() const -> IDispatcher& override;
    d1::HWND getWindowHandle() const override { return windowHandle_; }

    //--

    void subClassWindow(d1::HWND w);
    // Replaces the window procedure of w with its own window procedure and
    // stores the original window procedure as its default window procedure.
    // Stores w as its new window handle.

    void setDefaultWindowProc(WNDPROC);

    void callDefProcNow(Message& msg) const;
    // Calls the DefProc (Windows term) of this window for msg and sets the
    // return value of msg to the return value of the DefProc call.
    // PRE: EnableOS has not been called on msg.
    //      CallDefProcNow has not been called for msg.

    void processMsg(Message&) const;

private:
    d1::HWND windowHandle_ = {};
    bool windowIsDestroyed_ = false;
    Dispatcher dispatcher_;
    WNDPROC defaultWindowProc_ = {};
    WindowHandleTable<Window>::Remover remover_;

    LRESULT windowProc(HWND, UINT, WPARAM, LPARAM);
    static LRESULT CALLBACK sharedWindowProc(HWND, UINT, WPARAM, LPARAM);
};


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

    void registerChild(ResizeAlgorithm&);

    void storePos();
    void newSize();

private:
    void onSettingChange(WM_SETTINGCHANGE_Msg);
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
