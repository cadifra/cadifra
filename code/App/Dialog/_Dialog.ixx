/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module App.Dialog;

import App.Command;
import App.IApplication;

import d1.types;
import d1.NoValueSemantics;
import d1.StackFlag;

import WinUtil.IWindow;
import WinUtil.MessageLoop;
import WinUtil.Messages;

import std;


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


export class SaveAsHandler: public d1::NoValueSemantics
{
public:
    virtual void saveAs(HWND owner) = 0;

protected:
    ~SaveAsHandler() = default;
};


export class ErrorSaveAsDialog: public ModalDialog
{
    using This = ErrorSaveAsDialog;

    WinUtil::ProcRegistrar pr_;
    ExecRegistrar er_;
    SaveAsHandler& sah_;
    std::wstring text_;

public:
    ErrorSaveAsDialog(
        SaveAsHandler&,
        const std::wstring& text);

private:
    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
    void onSaveAs(const WinUtil::WM_COMMAND_Msg&);
};


export namespace DialogLayout
{

class DlgItem
{
    HWND dlg_;

public:
    DlgItem(HWND d);
    HWND operator()(int id) const;
};


LONG width(const RECT& r);
LONG height(const RECT& r);
SIZE getSize(const RECT& r); // Returns the width and height of r
SIZE makeSize(LONG x, LONG y);


SIZE convertDialogUnits(HWND w, const SIZE& s);
// Converts the dialog units s in pixel coordinates


RECT resizeClientRect(HWND w, const SIZE& s);
// Enlarges the window w such that its client rect equals s.
// The upper left coordinate of w is not changed.
// Returns the new window rect of w in client coordinates of
// the parent of w.


enum class Corner
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};


SIZE moveInside(HWND t, Corner ct, HWND r, Corner cr, const SIZE& d);
// Moves the window "t" such that its window rect corner "ct"
// gets the distance "d" to the client rect corner "cr" of "r".
// Returns the offset, the window "t" was moved.


SIZE moveAbsolute(HWND t, Corner ct, const RECT& r, Corner cr, const SIZE& d);
// Moves the window "t" such that its window rect corner "ct" gets the
// distance "d" to the corner "cr" of the RECT "r" ("r" in screen coordinates).
// Returns the offset, the window "t" was moved.


void moveDelta(HWND t, const SIZE& d);
// Moves the window "t" "d.cx" pixels to the right and "d.cy" pixels to the bottom


void defaultPlace(HWND dlg);
// Tries to place dlg's window nicely.

}


export namespace TabCtrl
{

// TabCtrl::IsDialogMessage is the same as ::IsDialogMessage but allows the user
// to switch between the tab pages with "CTRL+TAB", "CTRL+SHIFT+TAB", "CTRL+PG_UP"
// and "CTRL_+PG_DOWN".

bool isDialogMessage_(
    d1::HWND hDlg,  // handle to dialog box
    int tabCtrlId,  // id of the tab control
    d1::LPMSG lpMsg // message to be checked
);


d1::RECT getDisplayRect(d1::HWND t);
// Returns the display area of the tab control "t" in screen coordinates

}

}
