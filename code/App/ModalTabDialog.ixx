/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module App.ModalTabDialog;

import App.ModelessDialog;
import App.ExecRegistrar;


namespace App
{

/*
ModalTabDialog implements the correct keyboard interface for modal
dialogs that contain a WC_TABCONTROL ("SysTabControl32").

Width CTRL+TAB, CTRL+SHIFT+TAB, CTRL+PAGE_UP and CTRL+PAGE_DOWN,
the user can switch between the tabs.

The ModalTabDialog is implemented as a ModelessDialog that contains
its own message loop. This is necessary because the modal dialog
functions of the OS have a message loop that call the ::IsDialogMessage
function which consumes the CTRL+TAB, CTRL+SHIFT+TAB, CTRL+PAGE_UP and
CTRL+PAGE_DOWN keyboard messages.
The message loop of the ModalTabDialog class uses the function
App::TabCtrl::IsDialogMessage instead of ::IsDialogMessage.
*/

export class ModalTabDialog: public ModelessDialog
{
    using This = ModalTabDialog;

public:
    ModalTabDialog(IApplication&);
    ModalTabDialog(const ModalTabDialog&) = delete;
    ModalTabDialog& operator=(const ModalTabDialog&) = delete;

    virtual ~ModalTabDialog() = default;

    int show(
        HWND owner,                // owner window handle
        const TCHAR* templateName, // resource name/id of the dialog template
        int tabCtrlId              // resource id of the tab control
    );
    // Returns IDCANCEL, IDOK or any other value that was passed to
    // closeDialog(int).

protected:
    void closeDialog(int result);
    virtual void closing(int result) {}

private:
    ExecRegistrar execReg_;
    int result_ = 0;
    bool itIsClosed = false;

    void onCANCEL(const WinUtil::WM_COMMAND_Msg&);
    void onOK(const WinUtil::WM_COMMAND_Msg&);
};

}

module : private;

import App.TabCtrl;

import d1.ScopeGuard;

import WinUtil.WindowDisabler;


namespace App
{

namespace
{
using C = ModalTabDialog;
}


C::ModalTabDialog(IApplication& app):
    ModelessDialog{ app },
    execReg_{ getCommander(), 0 }
{
    auto eh = execReg_.helper(*this);

    eh.add(IDCANCEL, &This::onCANCEL);
    eh.add(IDOK, &This::onOK);
}


void C::onCANCEL(const WinUtil::WM_COMMAND_Msg&)
{
    closeDialog(IDCANCEL);
}


void C::onOK(const WinUtil::WM_COMMAND_Msg&)
{
    closeDialog(IDOK);
}


void C::closeDialog(int result)
{
    closing(result);
    HWND w = ::GetWindow(getWindowHandle(), GW_OWNER);
    if (w)
        ::EnableWindow(w, TRUE);

    result_ = result;
    itIsClosed = true;
}


int C::show(HWND owner, const TCHAR* templateName, int tabCtrlId)
{
    auto wd = WinUtil::WindowDisabler{};
    itIsClosed = false;
    result_ = 0;

    auto closeIt = d1::ScopeGuard{ [=] { close(); } };

    create(owner, templateName);

    for (;;) // Message Loop
    {
        if (itIsClosed or not getWindowHandle())
            break;

        auto msg = MSG{};
        BOOL res = ::GetMessage(&msg, 0, 0, 0);

        if (res == 0 and msg.message == WM_QUIT)
        {
            ::PostQuitMessage(static_cast<int>(msg.wParam)); // repost quit message
            break;
        }

        if (res == -1) // error
            break;

        if (not App::TabCtrl::isDialogMessage_(
                getWindowHandle(), tabCtrlId, &msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    return result_;
}

}
