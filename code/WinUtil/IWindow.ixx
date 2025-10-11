/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil:IWindow;

import :Messages;
import :ProcRegistrar;

import d1.Rect;

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


export class WinMsgDispatcher
{
public:
    WinMsgDispatcher();

    WinMsgDispatcher(const WinMsgDispatcher&) = delete;
    WinMsgDispatcher& operator=(const WinMsgDispatcher&) = delete;

    ~WinMsgDispatcher(); // intentionally not virtual

    operator IWinMsgDispatcher&() const;

    void Dispatch(WinMsg&, IPrePostDispatchObserver&) const;

private:
    class Impl;
    std::shared_ptr<Impl> itsImpl;
};


export class IWindow
{
public:
    virtual HWND GetWindowHandle() const = 0;

    virtual IWinMsgDispatcher& GetMsgDispatcher() const = 0;

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
    using Entry = std::pair<HWND, T*>;
    std::vector<Entry> itsTable;

    void Remove(HWND h)
    {
        auto it = std::ranges::find(itsTable, h, &Entry::first);
        if (it != end(itsTable))
            itsTable.erase(it);
    }

public:
    class DoRemove;
    using Remover = std::unique_ptr<DoRemove>;

    auto Add(HWND h, T* p) -> Remover
    {
        itsTable.push_back({ h, p });
        return std::make_unique<DoRemove>(this, h);
    }

    T* Find(HWND h)
    {
        auto it = std::ranges::find(itsTable, h, &Entry::first);
        return (it != end(itsTable)) ? it->second : nullptr;
    }

    WindowHandleTable() = default;
    WindowHandleTable(const WindowHandleTable&) = delete;
    WindowHandleTable& operator=(const WindowHandleTable&) = delete;

    class DoRemove
    {
        WindowHandleTable* itsWHT;
        HWND itsHWND;

    public:
        DoRemove(WindowHandleTable* t, HWND w):
            itsWHT{ t }, itsHWND{ w }
        {
        }
        ~DoRemove() { itsWHT->Remove(itsHWND); }
    };
};


export class Window: public IWindow
{
public:
    Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    virtual ~Window();

    //-- IWindow

    auto GetMsgDispatcher() const -> IWinMsgDispatcher& override;
    HWND GetWindowHandle() const override { return itsWindowHandle; }

    //--

    void SubClassWindow(HWND w);
    // Replaces the window procedure of w with its own window procedure and
    // stores the original window procedure as its default window procedure.
    // Stores w as its new window handle.

    void SetDefaultWindowProc(WNDPROC);

    void CallDefProcNow(WinMsg& msg) const;
    // Calls the DefProc (Windows term) of this window for msg and sets the
    // return value of msg to the return value of the DefProc call.
    // PRE: EnableOS has not been called on msg.
    //      CallDefProcNow has not been called for msg.

    void ProcessMsg(WinMsg&) const;

private:
    HWND itsWindowHandle = {};
    bool itsWindowIsDestroyed = false;
    WinMsgDispatcher itsDispatcher;
    WNDPROC itsDefaultWindowProc = {};
    WindowHandleTable<Window>::Remover itsRemover;

    LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);
    static LRESULT CALLBACK SharedWindowProc(HWND, UINT, WPARAM, LPARAM);
};


export class WindowResizer
{
public:
    class ResizeAlgorithm
    {
    public:
        virtual HDWP CallDeferWindowPos(HDWP dwp,
            const RECT& oldParent, const RECT& newParent) = 0;

        virtual int StorePos() = 0;

    protected:
        ~ResizeAlgorithm() = default;
    };


    WindowResizer(IWindow& theWindow);

    WindowResizer(const WindowResizer&) = delete;
    WindowResizer& operator=(const WindowResizer&) = delete;

    void RegisterChild(ResizeAlgorithm&);

    void StorePos();
    void NewSize();

    // Standard algorithms:

    struct Left
    {
        static LONG x(const RECT& r) { return r.left; }
    };
    struct Top
    {
        static LONG y(const RECT& r) { return r.top; }
    };
    struct Right
    {
        static LONG x(const RECT& r) { return r.right; }
    };
    struct Bottom
    {
        static LONG y(const RECT& r) { return r.bottom; }
    };


    template <class H = Left, class V = Top>
    class RelativeMover: public ResizeAlgorithm
    {
        HWND itsChild;
        RECT itsOldPos;

    public:
        RelativeMover(HWND child):
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

    template <
        class L = Left,
        class T = Top,
        class R = Right,
        class B = Bottom>
    class Stretcher: public ResizeAlgorithm
    {
        HWND itsChild;
        RECT itsOldPos;

    public:
        Stretcher(HWND child):
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

}
