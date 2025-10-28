/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.IWindow;

import WinUtil.Message;
import WinUtil.Messages;
import WinUtil.ProcRegistrar;

import d1.Rect;
import d1.wintypes;

import std;


namespace WinUtil
{

export class IPrePostDispatchObserver
{
public:
    virtual void PreDispatchNotification() = 0;
    // PreDispatchNotification is called before any function registered
    // for the dispatched message is called.

    virtual void PostDispatchNotification() = 0;
    // PostDispatchNotification is called after the last function registered
    // for the dispatched message has called.
    // PostDispatchNotification is never called if not PreDispatchNotification
    // has been called before.
    // PostDispatchNotification is guaranteed to be called if
    // PreDispatchNotification has been called before (also if an exception
    // occurs).

protected:
    ~IPrePostDispatchObserver() = default;
};


export class MessageDispatcher
{
public:
    MessageDispatcher();

    MessageDispatcher(const MessageDispatcher&) = delete;
    MessageDispatcher& operator=(const MessageDispatcher&) = delete;

    ~MessageDispatcher(); // intentionally not virtual

    operator IMessageDispatcher&() const;

    void Dispatch(Message&, IPrePostDispatchObserver&) const;

private:
    class Impl;
    std::shared_ptr<Impl> itsImpl;
};


export class IWindow
{
public:
    virtual d1::HWND GetWindowHandle() const = 0;

    virtual IMessageDispatcher& GetMsgDispatcher() const = 0;

    bool GetRect(d1::Rect& r) const;
    // Writes the window rect in screen coordinates to r and returns true.
    // Returns false if the call fails

    void UpdateWindow() const;
    void SetWindowTitle(const std::wstring& title);

protected:
    ~IWindow() = default;
};


export template <typename T>
class WindowHandleTable
{
    using Entry = std::pair<d1::HWND, T*>;
    std::vector<Entry> itsTable;

    void Remove(d1::HWND h)
    {
        auto it = std::ranges::find(itsTable, h, &Entry::first);
        if (it != end(itsTable))
            itsTable.erase(it);
    }

public:
    class DoRemove;
    using Remover = std::unique_ptr<DoRemove>;

    auto Add(d1::HWND h, T* p) -> Remover
    {
        itsTable.push_back({ h, p });
        return std::make_unique<DoRemove>(this, h);
    }

    T* Find(d1::HWND h)
    {
        auto it = std::ranges::find(itsTable, h, &Entry::first);
        return (it != end(itsTable)) ? it->second : nullptr;
    }

    WindowHandleTable() = default;
    WindowHandleTable(const WindowHandleTable&) = delete;
    WindowHandleTable& operator=(const WindowHandleTable&) = delete;
};


export template <typename T>
class WindowHandleTable<T>::DoRemove
{
    WindowHandleTable* itsWHT;
    d1::HWND itsHWND;

public:
    DoRemove(WindowHandleTable* t, d1::HWND w):
        itsWHT{ t }, itsHWND{ w }
    {
    }
    ~DoRemove() { itsWHT->Remove(itsHWND); }
};


export class Window: public IWindow
{
public:
    Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    virtual ~Window();

    //-- IWindow

    auto GetMsgDispatcher() const -> IMessageDispatcher& override;
    d1::HWND GetWindowHandle() const override { return itsWindowHandle; }

    //--

    void SubClassWindow(d1::HWND w);
    // Replaces the window procedure of w with its own window procedure and
    // stores the original window procedure as its default window procedure.
    // Stores w as its new window handle.

    void SetDefaultWindowProc(WNDPROC);

    void CallDefProcNow(Message& msg) const;
    // Calls the DefProc (Windows term) of this window for msg and sets the
    // return value of msg to the return value of the DefProc call.
    // PRE: EnableOS has not been called on msg.
    //      CallDefProcNow has not been called for msg.

    void ProcessMsg(Message&) const;

private:
    d1::HWND itsWindowHandle = {};
    bool itsWindowIsDestroyed = false;
    MessageDispatcher itsDispatcher;
    WNDPROC itsDefaultWindowProc = {};
    WindowHandleTable<Window>::Remover itsRemover;

    LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);
    static LRESULT CALLBACK SharedWindowProc(HWND, UINT, WPARAM, LPARAM);
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

    void RegisterChild(ResizeAlgorithm&);

    void StorePos();
    void NewSize();

private:
    void OnSettingChange(WM_SETTINGCHANGE_Msg);
    void OnWinPosChanged(WM_WINDOWPOSCHANGED_Msg);

    using This = WindowResizer;
    IWindow& itsWindow;
    ProcRegistrar itsProcReg;

    using Vector = std::vector<ResizeAlgorithm*>;
    Vector itsAlgorithms;
    RECT itsBasePos = {};
};


class WindowResizer::ResizeAlgorithm
{
public:
    virtual HDWP CallDeferWindowPos(HDWP dwp,
        const RECT& oldParent, const RECT& newParent) = 0;

    virtual int StorePos() = 0;

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
    d1::HWND itsChild;
    RECT itsOldPos;

public:
    RelativeMover(d1::HWND child):
        itsChild{ child }
    {
        StorePos();
    }

    HDWP CallDeferWindowPos(HDWP hdwp,
        const RECT& oldParent, const RECT& newParent)
    {
        RECT r = itsOldPos;

        OffsetRect(&r,
            H::x(newParent) - H::x(oldParent),
            V::y(newParent) - V::y(oldParent));

        MapWindowPoints(0, GetParent(itsChild), reinterpret_cast<POINT*>(&r), 2);

        return DeferWindowPos(hdwp, itsChild, 0, r.left, r.top, 0, 0,
            SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }

    int StorePos()
    {
        return GetWindowRect(itsChild, &itsOldPos);
    }
};


template <class L = Left, class T = Top, class R = Right, class B = Bottom>
class WindowResizer::Stretcher: public ResizeAlgorithm
{
    d1::HWND itsChild;
    RECT itsOldPos;

public:
    Stretcher(d1::HWND child):
        itsChild{ child }
    {
        StorePos();
    }

    HDWP CallDeferWindowPos(HDWP hdwp,
        const RECT& oldParent, const RECT& newParent)
    {
        auto rc = RECT{ itsOldPos };

        rc.left += L::x(newParent) - L::x(oldParent);
        rc.top += T::y(newParent) - T::y(oldParent);
        rc.right += R::x(newParent) - R::x(oldParent);
        rc.bottom += B::y(newParent) - B::y(oldParent);

        MapWindowPoints(0, GetParent(itsChild), reinterpret_cast<POINT*>(&rc), 2);

        return DeferWindowPos(hdwp, itsChild, 0,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOZORDER);
    }

    int StorePos()
    {
        return GetWindowRect(itsChild, &itsOldPos);
    }
};

}
