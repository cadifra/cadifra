/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

export module App.IWindow;

import App.NumberedWindow;
import App.LifeTimeSupervisor;
import App.IApplication;

import WinUtil.Window;
import WinUtil.Messages;

import std;


namespace App
{

export class IWindow:
    public WinUtil::Window,
    public INumberedWindow,
    public LifeTimeSupervisor::Client
{
    using This = IWindow;

public:
    class Creator;

    IWindow(Creator&, LifeTimeSupervisor& lts);
    IWindow(const IWindow&) = delete;
    IWindow& operator=(const IWindow&) = delete;

    bool askForUserClose();

    auto getApplication() const -> IApplication&;

    bool isActive() const { return itIsActive; }

    auto getLifeTimeSupervisor() -> LifeTimeSupervisor&;

    //-- ILifeTimeSupervisorClient

    void shutDown() override;

    //--

private:
    Creator& creator_;
    LifeTimeSupervisor& lifeTimeSupervisor_;
    bool itIsActive = true;

    virtual void changeActiveState(bool isActive) = 0;

    WinUtil::ProcRegistrar registrar_;
    void onNCActivate(WinUtil::WM_NCACTIVATE_Msg);
    void onDestroy(WinUtil::WM_DESTROY_Msg);
    void onClose(WinUtil::WM_CLOSE_Msg);
    void onQueryEndSession(WinUtil::WM_QUERYENDSESSION_Msg);
};


class IWindow::Creator
{
public:
    virtual int getWindowCount() const = 0;
    virtual void forget(IWindow&) = 0;

    virtual auto getApplication() const -> IApplication& = 0;

    virtual void changeActiveState(bool isActive) = 0;

    virtual bool askForUserClose() = 0;

    virtual void setLastActiveWindow(IWindow&) = 0;

protected:
    ~Creator() = default;
};


export class IWindowVisitor
{
public:
    virtual bool visit(IWindow&) = 0;

protected:
    ~IWindowVisitor() = default;
};

}

module : private;


namespace App
{

namespace
{
using C = IWindow;
}


C::IWindow(
    Creator& creator,
    LifeTimeSupervisor& lts):

    creator_{ creator },
    lifeTimeSupervisor_{ lts },
    registrar_{ getDispatcher(), 0 }
{
    auto r = registrar_.helper(*this);

    r.addAlwaysReady(&This::onDestroy);
    r.addAlwaysReady(&This::onClose);
    r.addAlwaysReady(&This::onNCActivate);
    r.addAlwaysReady(&This::onQueryEndSession);

    lifeTimeSupervisor_.lock();
}


bool C::askForUserClose()
{
    if (creator_.getWindowCount() == 1)
        return creator_.askForUserClose();
    else
        return true;
}


void C::onDestroy(WinUtil::WM_DESTROY_Msg)
{
    lifeTimeSupervisor_.forceShutDown();
    // For the case that someone brutally destroyed the window without triggering
    // the shut down mechanism. If the shut down mechanism is running, triggering
    // it a second time doesn't matter.
}


void C::onClose(WinUtil::WM_CLOSE_Msg)
{
    if (askForUserClose())
        lifeTimeSupervisor_.forceShutDown();
}


void C::onQueryEndSession(WinUtil::WM_QUERYENDSESSION_Msg msg)
{
    if (askForUserClose())
        msg.setResult(TRUE);
    else
        msg.setResult(FALSE);
}


void C::shutDown()
{
    // creator_.getMdiFrame().Show(false);
    //  ::SendMessage(getWindowHandle(), WM_CLOSE, 0, 0);
    //  Sending WM_CLOSE to a closed window doesn't matter. But destroying a window
    //  without using the message WM_CLOSE will prohibit the mdi client window to
    //  activate the next child window after the active one has disappeared.

    creator_.forget(*this);
    ::DestroyWindow(getWindowHandle());
}


void C::onNCActivate(WinUtil::WM_NCACTIVATE_Msg msg)
{
    itIsActive = (msg.fActive() != 0);

    if (itIsActive)
        creator_.setLastActiveWindow(*this);

    creator_.changeActiveState(itIsActive);
    changeActiveState(itIsActive);

    msg.enableOS();
}


auto C::getApplication() const -> IApplication&
{
    return creator_.getApplication();
}


auto C::getLifeTimeSupervisor() -> LifeTimeSupervisor&
{
    return lifeTimeSupervisor_;
}

}
