/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module App.Dialog;

import App.Command;
import App.Commander;

import WinUtil.IWindow;
import WinUtil.Messages;
import WinUtil.Dispatcher;


namespace App
{

export class Dialog: public WinUtil::IWindow
{
    using This = Dialog;

public:
    Dialog();
    Dialog(const Dialog&) = delete;
    Dialog& operator=(const Dialog&) = delete;
    virtual ~Dialog();

    //-- WinUtil::IWindow

    auto getDispatcher() const -> WinUtil::IDispatcher& override;

    //--

    auto getCommander() const -> ICommander&;

    virtual void process(WinUtil::Message&);

private:
    std::unique_ptr<WinUtil::Dispatcher> dispatcher_;
    std::unique_ptr<Commander> commander_;

    WinUtil::ProcRegistrar procReg_;
    void onSysColorChange(WinUtil::WM_SYSCOLORCHANGE_Msg);
    void onSettingChange(WinUtil::WM_SETTINGCHANGE_Msg);
};

}

module : private;

import WinUtil.CursorManager;
import WinUtil.visitChildWindows;


namespace App
{

namespace
{
using C = Dialog;
}


C::Dialog():
    dispatcher_{ std::make_unique<WinUtil::Dispatcher>() },
    procReg_{ *dispatcher_, 0 }
{
    commander_ = std::make_unique<Commander>(*this);

    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onSysColorChange);
    ph.addSpy(&This::onSettingChange);
}


C::~Dialog()
{
}


auto C::getDispatcher() const -> WinUtil::IDispatcher&
{
    return *dispatcher_;
}


auto C::getCommander() const -> ICommander&
{
    return commander_.get()->get();
}


void C::process(WinUtil::Message& msg)
{
    class WCS: public WinUtil::IPrePostDispatchObserver
    {
        WinUtil::CursorManager::WaitCursorSwitch switch_;
        void preDispatchNotification() final { switch_.on(); }
        void postDispatchNotification() final { switch_.off(); }
    } wcs;

    dispatcher_->dispatch(msg, wcs);
}


void C::onSysColorChange(WinUtil::WM_SYSCOLORCHANGE_Msg msg)
{
    auto v = WinUtil::ChildWindowSender{
        WM_SYSCOLORCHANGE, msg.getWParam(), msg.getLParam()
    };

    WinUtil::visitChildWindows(getWindowHandle(), &v);
}


void C::onSettingChange(WinUtil::WM_SETTINGCHANGE_Msg msg)
{
    auto v = WinUtil::ChildWindowSender{
        WM_SETTINGCHANGE, msg.getWParam(), msg.getLParam()
    };

    WinUtil::visitChildWindows(getWindowHandle(), &v);
}

}
