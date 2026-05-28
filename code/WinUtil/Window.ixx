/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module WinUtil.Window;

import WinUtil.IWindow;
import WinUtil.Messages;
import WinUtil.Dispatcher;

import d1.wintypes;

import std;


namespace WinUtil
{

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

}


module : private;

import WinUtil.CursorManager;
import WinUtil.GuardedFunctionCall;


namespace WinUtil
{

WindowHandleTable<Window> sharedWindowHandleTable;

using C = Window;


C::Window():
    defaultWindowProc_{ &::DefWindowProc }
{
}


C::~Window()
{
    if (windowHandle_ and not windowIsDestroyed_)
        ::DestroyWindow(windowHandle_);
}


void C::processMsg(Message& msg) const
{
    class WaitCursorSwitcher: public WinUtil::IPrePostDispatchObserver
    {
        CursorManager::WaitCursorSwitch switch_;
        void preDispatchNotification() final { switch_.on(); }
        void postDispatchNotification() final { switch_.off(); }
    };

    auto wcs = WaitCursorSwitcher{};

    const HWND h = windowHandle_;
    const WNDPROC p = defaultWindowProc_;

    dispatcher_.dispatch(msg, wcs); // may delete this!

    if (msg.OSisEnabled() and not msg.defProcCalled())
    {
        msg.defProcCalled(::CallWindowProc(p,
            h, msg.getMsgId(), msg.getWParam(), msg.getLParam()));
    }
}


auto C::getDispatcher() const -> IDispatcher&
{
    return dispatcher_;
}


void C::callDefProcNow(Message& msg) const
{
    msg.defProcCalled(::CallWindowProc(defaultWindowProc_,
        windowHandle_, msg.getMsgId(), msg.getWParam(), msg.getLParam()));
}


void C::setDefaultWindowProc(WNDPROC p)
{
    defaultWindowProc_ = p;
}


void C::subClassWindow(HWND hwnd)
{
    D1_ASSERT(not windowIsDestroyed_);
    windowHandle_ = hwnd;
    remover_ = sharedWindowHandleTable.add(hwnd, this);

    defaultWindowProc_ =
        reinterpret_cast<WNDPROC>(
            ::SetWindowLongPtr(
                hwnd,
                GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(sharedWindowProc)));
}


LRESULT C::windowProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    windowHandle_ = hwnd;

    if (uMsg == WM_DESTROY)
    {
        D1_ASSERT(not windowIsDestroyed_);
        windowIsDestroyed_ = true;
    }
    else if (uMsg == WM_NCDESTROY)
        remover_.reset();

    auto msg = Message{ uMsg, wp, lp };

    processMsg(msg);

    return msg.getLResult();
}


LRESULT CALLBACK C::sharedWindowProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    Window* w = sharedWindowHandleTable.find(hwnd);

    if (not w)
        return ::DefWindowProc(hwnd, uMsg, wp, lp);

    return GuardedCallHelpers::call(
        "WinUtil::Window",
        *w,
        &Window::windowProc,
        hwnd, uMsg, wp, lp,
        LRESULT(0));
}

}
