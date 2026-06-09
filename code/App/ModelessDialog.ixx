/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module App.ModelessDialog;

import App.Dialog;
import App.IApplication;

import d1.StackFlag;

import WinUtil.MessageLoop;
import WinUtil.Window;
import WinUtil.ProcRegistrar;


namespace App
{

export class ModelessDialog:
    public Dialog,
    private WinUtil::MessageLoop::IPreProc
{
public:
    ModelessDialog(IApplication&);
    ModelessDialog(const ModelessDialog&) = delete;
    ModelessDialog& operator=(const ModelessDialog&) = delete;

    ~ModelessDialog() override;

    //-- WinUtil::IWindow

    HWND getWindowHandle() const override { return dlgHandle_; }

    //--

    void close();

    void create(HWND owner, const TCHAR* templateName);
    // creates the dialog window if it does not yet exist

    IApplication& getApplication() const { return app_; }

private:
    using This = ModelessDialog;

    HWND dlgHandle_ = {};
    d1::StackFlag::Ref itIsCreating;
    IApplication& app_;
    WinUtil::WindowHandleTable<This>::Remover remover_;


    WinUtil::ProcRegistrar procReg_;

    INT_PTR dialogProc(HWND, UINT, WPARAM, LPARAM);
    static INT_PTR guardedSharedDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK sharedDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //-- WinUtil::MessageLoop::IPreProc

    bool preProcess(MSG&) override;

    //--

    void onDestroy(WinUtil::WM_DESTROY_Msg);
};

}

module : private;


import WinUtil.GuardedFunctionCall;


namespace App
{

namespace
{
WinUtil::WindowHandleTable<ModelessDialog> sharedWindowHandleTable;

using C = ModelessDialog;
}


C::ModelessDialog(IApplication& app):
    app_{ app },
    procReg_{ getDispatcher(), 0 }
{
    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onDestroy);
}


C::~ModelessDialog()
{
    close();
}


void C::create(HWND owner, const TCHAR* templateName)
{
    if (dlgHandle_)
        return;

    auto stackFlag = d1::StackFlag{ itIsCreating };

    ::CreateDialogParam(
        app_.getInstanceHandle(),
        templateName,
        owner,
        reinterpret_cast<DLGPROC>(sharedDialogProc),
        reinterpret_cast<LPARAM>(this));

    D1_ASSERT(dlgHandle_);
    IPreProc::set(app_.getMessageLoop());
}


void C::close()
{
    if (dlgHandle_)
        D1_VERIFY(::DestroyWindow(dlgHandle_));
}


INT_PTR C::dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (itIsCreating and not dlgHandle_)
        dlgHandle_ = hwnd;

    auto msg = WinUtil::Message{ uMsg, wParam, lParam };

    process(msg);

    if (msg.resultSet())
        return static_cast<INT_PTR>(msg.getLResult());

    if (msg.OSisEnabled())
        return FALSE;
    else
        return TRUE;
}


INT_PTR C::guardedSharedDialogProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        auto d = reinterpret_cast<ModelessDialog*>(lParam);
        d->remover_ = sharedWindowHandleTable.add(hwnd, d);
    }

    ModelessDialog* d = sharedWindowHandleTable.find(hwnd);

    if (not d)
        return FALSE;

    if (uMsg == WM_NCDESTROY)
        d->remover_.reset();

    return d->dialogProc(hwnd, uMsg, wParam, lParam);
}


INT_PTR CALLBACK C::sharedDialogProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return WinUtil::GuardedCallHelpers::call(
        "App::ModelessDialog",
        &guardedSharedDialogProc,
        hwnd, uMsg, wParam, lParam,
        INT_PTR(0));
}


void C::onDestroy(WinUtil::WM_DESTROY_Msg)
{
    IPreProc::clear();
    dlgHandle_ = 0;
}


bool C::preProcess(MSG& msg)
{
    return 0 != ::IsDialogMessage(dlgHandle_, &msg);
}

}
