/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module App.Dialog;

import App.ExecRegistrar;
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


export class ISettingChanged
{
public:
    virtual void settingChanged() = 0;

    class Bookkeeper;

protected:
    ~ISettingChanged() = default;
};


class ISettingChanged::Bookkeeper
{
    ISettingChanged& observer_;

public:
    Bookkeeper(ISettingChanged& o); // registers o
    ~Bookkeeper();                  // unregisters o

    Bookkeeper(const Bookkeeper&) = delete;
    Bookkeeper& operator=(const Bookkeeper&) = delete;
};


template <class EditCtrl>
class EditCtrlHelper
{
public:
    class IObserver;

    EditCtrlHelper(IObserver* o):
        observer_{ o }
    {
    }

    void notifyObserver(EditCtrl&);
    bool isNotifying() const { return itIsNotifying; }

    class SuppressNotifications;

private:
    IObserver* observer_;
    d1::StackFlag::Ref itIsNotifying;
};


template <class EditCtrl>
class EditCtrlHelper<EditCtrl>::IObserver
{
public:
    virtual void changeNotification(EditCtrl&) = 0;
};


template <class EditCtrl>
class EditCtrlHelper<EditCtrl>::SuppressNotifications
{
    EditCtrlHelper& ECH_;
    IObserver* oldObs_;

public:
    SuppressNotifications(EditCtrlHelper& ech):
        ECH_{ ech }, oldObs_{ ech.observer_ }
    {
        ech.observer_ = 0;
    }
    ~SuppressNotifications()
    {
        ECH_.observer_ = oldObs_;
    }
};


template <class EditCtrl>
void EditCtrlHelper<EditCtrl>::notifyObserver(EditCtrl& e)
{
    if (not observer_)
        return;

    auto stackFlag = d1::StackFlag{ itIsNotifying };

    observer_->changeNotification(e);
}


/*
A LengthEditCtrl manages an edit control of a dialog. The control allows
the user to enter a length with optional units.
*/

export class LengthEditCtrl: private ISettingChanged
{

public:
    using IObserver = EditCtrlHelper<LengthEditCtrl>::IObserver;

    enum Units
    {
        FOOT,
        INCH,
        METER,
        CENTIMETER,
        MILLIMETER
    };

    LengthEditCtrl(const LengthEditCtrl&) = delete;
    LengthEditCtrl& operator=(const LengthEditCtrl&) = delete;

    virtual ~LengthEditCtrl() = default;

    d1::float64 getValue() const;        // returns the current value in MILLIMETER
    void setValue(const d1::float64& f); // f in MILLIMETER
    // "SetValue" does not notify observer_.
    // "SetValue" calls are ignored while notifying observer_

    enum class State
    {
        VALID,
        INVALIDNUMBER,
        INVALIDUNITS,
        TOBIG,
        TOSMALL
    };
    State getState() const;

    static d1::float64 convert(const d1::float64& f, Units from, Units to);

private:
    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
    void onCommand(WinUtil::WM_COMMAND_Msg);
    void onNotify(WinUtil::WM_NOTIFY_Msg);

    std::wstring getString() const;
    void setString(const std::wstring&);
    void showValue();
    void evaluate();
    void internalSetValue(const d1::float64& f); // f in MILLIMETER

    Units getUserSettingUnits() const;

    void settingChanged() override;

private:
    using This = LengthEditCtrl;
    WinUtil::ProcRegistrar procReg_;
    EditCtrlHelper<This> editCtrlHelper_;
    IApplication& app_;
    Dialog& dlg_;
    const int ctrlId_;
    d1::HWND ctrl_ = {};
    d1::HWND upDown_ = {};
    d1::float64 value_;             // in MILLIMETER
    const d1::float64 min_, max_; // in MILLIMETER
    const d1::float64 delta_;
    const int precision_;
    const Units SIUnits_;
    const Units USsystemUnits_;
    d1::StackFlag::Ref ignoreFlag_;
    State state_ = State::VALID;
    Units displayUnits_;
    ISettingChanged::Bookkeeper bookkeeper_ = { *this };

public:
    // Call the constructor before the dialog opens.
    LengthEditCtrl(
        IApplication& app,
        Dialog& dlg,
        int control,                     // identifier of the control
        IObserver* observer,             // 0 if not used
        const d1::float64& initialValue, // in MILLIMETER
        const d1::float64& minValue,     // in MILLIMETER
        const d1::float64& maxValue,     // in MILLIMETER
        const d1::float64& deltaValue = 0.1,
        int precision = 4,
        Units SIUnits = CENTIMETER,
        Units USsystemUnits = INCH):

        procReg_{ dlg.getDispatcher(), 0 },
        editCtrlHelper_{ observer },
        app_{ app },
        dlg_{ dlg },
        ctrlId_{ control },
        value_{ initialValue },
        min_{ minValue },
        max_{ maxValue },
        delta_{ deltaValue },
        precision_{ precision },
        SIUnits_{ SIUnits },
        USsystemUnits_{ USsystemUnits },
        displayUnits_{ SIUnits }
    {
        D1_ASSERT(min_ < max_);
        init();
    }

    void init();
};


/*
A PercentEditCtrl manages an edit control of a dialog. The control allows
the user to enter a percent value.
*/

export class PercentEditCtrl
{
public:
    using IObserver = EditCtrlHelper<PercentEditCtrl>::IObserver;


    PercentEditCtrl(
        IApplication& app,
        Dialog& dlg,
        int control,                     // identifier of the control
        const d1::float64& minValue,     // in default units
        const d1::float64& maxValue,     // in default units
        const d1::float64& initialValue, // in default units
        const d1::float64& deltaValue,   // in default units
        IObserver* observer = 0);
    // Call the constructor before the dialog opens.

    PercentEditCtrl(const PercentEditCtrl&) = delete;
    PercentEditCtrl& operator=(const PercentEditCtrl&) = delete;

    virtual ~PercentEditCtrl() = default;

    d1::float64 getValue();

    void setValue(const d1::float64&);
    // "SetValue" does not notify observer_.
    // "SetValue" calls are ignored while notifying observer_

    enum class State
    {
        VALID,
        INVALIDNUMBER,
        TOBIG,
        TOSMALL
    };
    State getState() const;

private:
    using This = PercentEditCtrl;
    WinUtil::ProcRegistrar procReg_;

    EditCtrlHelper<This> editCtrlHelper_;

    IApplication& app_;
    Dialog& dlg_;
    const int ctrlId_;
    d1::HWND ctrl_ = {};
    d1::HWND upDown_ = {};
    const d1::float64 min_, max_, delta_;
    d1::float64 lastValidVal_;

    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
    void onCommand(WinUtil::WM_COMMAND_Msg);
    void onNotify(WinUtil::WM_NOTIFY_Msg);

    std::wstring getString() const;
    void setString(const std::wstring&) const;

    void internalSetValue(const d1::float64&);
};


/*
An IntEditCtrl manages an edit control of a dialog. The control allows
the user to enter an integer between a given range. The range is checked
during typing. The IntEditCtrl will add an up/down control to the given
edit control of the dialog.
*/

export class IntEditCtrl
{
public:
    class IObserver;

    IntEditCtrl(
        IApplication& app,
        Dialog& dlg,
        int control, // identifier of the control
        int minValue,
        int maxValue,
        int initialValue,
        IObserver* observer = 0);
    // Call the constructor before the dialog opens.

    IntEditCtrl(const IntEditCtrl&) = delete;
    IntEditCtrl& operator=(const IntEditCtrl&) = delete;

    virtual ~IntEditCtrl() = default;

    int getValue() const;
    void setValue(int);

    void enable(bool enable);

private:
    using This = IntEditCtrl;
    WinUtil::ProcRegistrar procReg_;

    IApplication& app_;
    Dialog& dlg_;
    const int ctrlId_;
    d1::HWND ctrl_ = {};
    d1::HWND upDown_ = {};
    const int min_, max_;
    int initialVal_;
    IObserver* observer_ = nullptr;

    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
    void onCommand(WinUtil::WM_COMMAND_Msg);

    bool check() const;
};


class IntEditCtrl::IObserver
{
public:
    virtual void changeNotification(IntEditCtrl&) = 0;

protected:
    ~IObserver() = default;
};


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
