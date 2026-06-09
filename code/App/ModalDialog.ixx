/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module App.ModalDialog;

import App.Dialog;
import App.IApplication;
import App.ExecRegistrar;

import WinUtil.Window;


namespace App
{

export class ModalDialog: public Dialog
{
public:
    ModalDialog();
    ModalDialog(const ModalDialog&) = delete;
    ModalDialog& operator=(const ModalDialog&) = delete;

    //-- WinUtil::IWindow

    HWND getWindowHandle() const override { return dlgHandle_; }
    // The returned handle is valid only during the Show call. Otherwise it is 0.

    //--

    int show(IApplication&, HWND owner, const TCHAR* templateName);
    // Displays the dialog box. The function returns, when the user has closed
    // the dialog box. The returned value is the "EndDialog" parameter.

protected:
    virtual void closing() {}

private:
    using This = ModalDialog;

    HWND dlgHandle_ = {};
    WinUtil::WindowHandleTable<This>::Remover remover_;
    ExecRegistrar execReg_;

    void onCANCEL(const WinUtil::WM_COMMAND_Msg&);
    void onOK(const WinUtil::WM_COMMAND_Msg&);

    INT_PTR dialogProc(HWND, UINT, WPARAM, LPARAM);
    static INT_PTR guardedSharedDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK sharedDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

}

module : private;

import WinUtil.GuardedFunctionCall;
import WinUtil.WindowDisabler;


namespace App
{

namespace
{
WinUtil::WindowHandleTable<ModalDialog> sharedWindowHandleTable;
}

namespace
{
using C = ModalDialog;
}


C::ModalDialog():
    execReg_{ getCommander(), 0 }
{
    auto eh = execReg_.helper(*this);

    eh.add(IDCANCEL, &This::onCANCEL);
    eh.add(IDOK, &This::onOK);
}


int C::show(IApplication& app, HWND owner, const TCHAR* templateName)
{
    auto wd = WinUtil::WindowDisabler{};

    auto res = static_cast<int>(::DialogBoxParam(
        app.getInstanceHandle(),
        templateName,
        owner,
        reinterpret_cast<DLGPROC>(sharedDialogProc),
        reinterpret_cast<LPARAM>(this)));

    dlgHandle_ = 0;
    return res;
}



void C::onCANCEL(const WinUtil::WM_COMMAND_Msg&)
{
    closing();
    D1_VERIFY(::EndDialog(dlgHandle_, IDCANCEL));
}


void C::onOK(const WinUtil::WM_COMMAND_Msg&)
{
    closing();
    D1_VERIFY(::EndDialog(dlgHandle_, IDOK));
}


INT_PTR C::dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (not dlgHandle_)
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
        auto d = reinterpret_cast<ModalDialog*>(lParam);
        d->remover_ = sharedWindowHandleTable.add(hwnd, d);
    }

    ModalDialog* d = sharedWindowHandleTable.find(hwnd);

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
        "App::ModalDialog",
        &ModalDialog::guardedSharedDialogProc,
        hwnd, uMsg, wParam, lParam,
        INT_PTR{ 0 });
}

}
