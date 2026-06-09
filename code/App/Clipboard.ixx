/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <objidl.h>

export module App.Clipboard;

import App.IDataHolder;
import App.DataProvider;

import d1.Observer;

import WinUtil.Window;
import WinUtil.ProcRegistrar;
import WinUtil.Messages;

import std;


namespace App
{

export class IClipboard: public IDataHolder
{
public:
    virtual void put(DataProvider&) = 0;
    // Puts the IDataObject from the DataProvider into the clipboard
    // if the clipboard isn't used by somebody else.

    virtual ~IClipboard() = default;
    IClipboard() = default;
    IClipboard(const IClipboard&) = delete;
    IClipboard& operator=(const IClipboard&) = delete;
};


export class Clipboard: public IClipboard
{
    ::IDataObject* lastPutObject_ = nullptr;

public:
    Clipboard() = default;
    virtual ~Clipboard();

    virtual void put(DataProvider&);
    virtual bool extract(IFormatExtractor&);
    virtual bool QueryExtract(IFormatTester&);
};


export class IClipboardChange
{
public:
    virtual void clipboardChanged() = 0;

protected:
    ~IClipboardChange() = default;
};


export class ClipboardViewer
{
    class Impl;
    std::unique_ptr<Impl> impl_;

public:
    ClipboardViewer(WinUtil::Window&);

    auto getConnector() -> d1::Observer::C<IClipboardChange>;
};


class ClipboardViewer::Impl
{
    using This = Impl;

    WinUtil::ProcRegistrar procReg_;

    d1::Observer::L<IClipboardChange> observerList_;

    HWND window_ = {};
    bool isListener_ = false;
    DWORD clipboardSequenceNumber_ = GetClipboardSequenceNumber();

    void addAsListener();
    void removeAsListener();
    void doNotify();

    void onClipboardUpdate(WinUtil::WM_CLIPBOARDUPDATE_Msg);
    void onActivate(WinUtil::WM_ACTIVATE_Msg);
    void onTimer(WinUtil::WM_TIMER_Msg);

public:
    Impl(WinUtil::Window&);
    ~Impl();

    auto getConnector() -> d1::Observer::C<IClipboardChange>;
};

}


module : private;

import d1.AutoComPtr;


namespace App
{

Clipboard::~Clipboard()
{
    if (lastPutObject_)
    {
        HRESULT res = ::OleIsCurrentClipboard(lastPutObject_);

        if (res == S_OK)
        {
            res = ::OleFlushClipboard();
            D1_ASSERT(res == S_OK);
        }
    }
}


void Clipboard::put(DataProvider& dp)
{
    auto o = d1::AutoComPtr<IDataObject>{};

    dp.GetDataObject(&o);

    HRESULT res = ::OleSetClipboard(o);
    if (S_OK == res)
        lastPutObject_ = o;
}


bool Clipboard::extract(IFormatExtractor& e)
{
    auto pData = d1::AutoComPtr<IDataObject>{};

    HRESULT res = ::OleGetClipboard(&pData);

    if (res == S_OK)
        return e.extract(pData);
    else
        return false;
}


bool Clipboard::QueryExtract(IFormatTester& t)
{
    auto pData = d1::AutoComPtr<IDataObject>{};

    HRESULT res = ::OleGetClipboard(&pData);

    if (res == S_OK)
        return t.QueryExtract(pData);
    else
        return false;
}


ClipboardViewer::Impl::Impl(WinUtil::Window& w):
    procReg_{ w.getDispatcher(), 0 },
    window_{ w.getWindowHandle() }
{
    auto ph = procReg_.helper(*this);

    ph.addAlwaysReady(&This::onClipboardUpdate);
    ph.addAlwaysReady(&This::onTimer);
    ph.addSpy(&This::onActivate);

    addAsListener();
}


ClipboardViewer::Impl::~Impl()
{
    removeAsListener();
}


void ClipboardViewer::Impl::addAsListener()
{
    if (isListener_)
        return;

    D1_VERIFY(AddClipboardFormatListener(window_));
    isListener_ = true;

    doNotify(); // clipboard could have changed while we weren't listening
}


void ClipboardViewer::Impl::removeAsListener()
{
    if (not isListener_)
        return;

    RemoveClipboardFormatListener(window_);
    isListener_ = false;
}


auto ClipboardViewer::Impl::getConnector() -> d1::Observer::C<IClipboardChange>
{
    return observerList_.getConnector();
}


void ClipboardViewer::Impl::onClipboardUpdate(WinUtil::WM_CLIPBOARDUPDATE_Msg)
{
    doNotify();
}


void ClipboardViewer::Impl::doNotify()
{
    const DWORD oldNumber = std::exchange(
        clipboardSequenceNumber_, GetClipboardSequenceNumber());

    if (oldNumber == clipboardSequenceNumber_)
        return;

    D1_VERIFY(SetTimer(window_, reinterpret_cast<UINT_PTR>(this), 100, 0));
}


void ClipboardViewer::Impl::onTimer(WinUtil::WM_TIMER_Msg)
{
    D1_VERIFY(KillTimer(window_, reinterpret_cast<UINT_PTR>(this)));

    observerList_.notify(
        [](IClipboardChange* obs) { obs->clipboardChanged(); });
}


void ClipboardViewer::Impl::onActivate(WinUtil::WM_ACTIVATE_Msg m)
{
    if (m.state())
        addAsListener();
    else
        removeAsListener();

    // Taskbar-Bug on Windows 11
    //
    // If you minimize one or more UML editor windows to the taskbar, restore
    // will not properly work. With a single window, clicking twice helps.
    //
    // The bug only shows up, if another running App puts a COM object
    // into the clipboard. Examples for such applications are Visual Studio,
    // OpenOffice, LibreOffice or cadifra.exe running in another process.
    //
    // The problem disappears as soon as that application stops running or any
    // other application puts some simple data into the clipboard.
    //
    // Commenting out (for experiment) the access to the IDataObject retrieved
    // with ::OleGetClipboard, removes the problem. Even asking QueryInterface
    // for IDetectInProcess (see IDetectInProcess.cpp) is enough to provoke the
    // bug.
    //
    // DelayedActivateMsg is a workaround. Delaying the clipboard access with
    // at least two message posts, prevents the bug for whatever reason.

    // Addendum:
    // The bug does not show up anymore, if the clipboard hasn't changed,
    // because we now check the ClipboardSequenceNumber. But it is still there.
    // Copy some text in Visual Studio and the first try restoring the
    // UML editor would fail without the workaround.

    // Addendum 2:
    // DelayedActivateMsg ist not enough delay. The bug still shows up if you
    // restore the UML editor via the small preview window that appears when
    // hovering over the application icon in the taskbar. That's why we use a
    // timer instead.
}


auto ClipboardViewer::getConnector() -> d1::Observer::C<IClipboardChange>
{
    return impl_->getConnector();
}


ClipboardViewer::ClipboardViewer(WinUtil::Window& w):
    impl_{ std::make_unique<Impl>(w) }
{
}

}
