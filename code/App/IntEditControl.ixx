/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <commctrl.h>
#include <math.h>

export module App.IntEditControl;

import App.IApplication;
import App.Dialog;

import d1.wintypes;

import WinUtil.ProcRegistrar;
import WinUtil.Messages;


namespace App
{

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

}


module : private;


namespace App
{

namespace
{
using C = IntEditCtrl;
}

C::IntEditCtrl(
    IApplication& app,
    Dialog& dlg,
    int control,
    int minValue,
    int maxValue,
    int initialValue,
    IObserver* observer):

    procReg_{ dlg.getDispatcher(), 0 },
    app_{ app },
    dlg_{ dlg },
    ctrlId_{ control },
    min_{ minValue },
    max_{ maxValue },
    initialVal_{ initialValue },
    observer_{ observer }
{
    D1_ASSERT(minValue < maxValue);
    D1_ASSERT(minValue >= 0);

    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onInitDialog);
    ph.addSpy(&This::onCommand);
}


void C::onInitDialog(WinUtil::WM_INITDIALOG_Msg)
{
    IObserver* oldObserver = observer_;
    observer_ = 0; // prevent notifications during InitDialog

    ctrl_ = ::GetDlgItem(dlg_.getWindowHandle(), ctrlId_);
    D1_ASSERT(ctrl_);

    upDown_ = ::CreateUpDownControl(
        WS_CHILD | WS_BORDER | WS_VISIBLE |
            UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT,
        0, 0, 0, 0,
        dlg_.getWindowHandle(),
        -1,
        app_.getInstanceHandle(),
        ctrl_,
        max_, min_, initialVal_);
    D1_ASSERT(upDown_);

    int len = 1 + static_cast<int>(::ceil(::log10(static_cast<double>(max_))));

    ::SendMessage(ctrl_, EM_LIMITTEXT, len, 0);

    observer_ = oldObserver;
}


void C::onCommand(WinUtil::WM_COMMAND_Msg msg)
{
    if (msg.hwndCtl() == ctrl_ and msg.wNotifyCode() == EN_CHANGE)
    {
        if (not check())
            ::SendMessage(ctrl_, EM_SETSEL, 0, -1);
        if (observer_)
            observer_->changeNotification(*this);
    }
}


bool C::check() const
{
    BOOL translated = false;

    int res = ::GetDlgItemInt(
        dlg_.getWindowHandle(),
        ctrlId_,   // control identifier
        &translated, // success state
        false        // not signed
    );

    if (not translated)
    {
        D1_VERIFY(::SetDlgItemInt(dlg_.getWindowHandle(), ctrlId_, min_, false));
        return false;
    }

    if (res < min_)
    {
        D1_VERIFY(::SetDlgItemInt(dlg_.getWindowHandle(), ctrlId_, min_, false));
        return false;
    }
    else if (res > max_)
    {
        D1_VERIFY(::SetDlgItemInt(dlg_.getWindowHandle(), ctrlId_, max_, false));
        return false;
    }

    return true;
}


int C::getValue() const
{
    if (not ctrl_)
        return initialVal_;

    BOOL translated = false;

    int res = ::GetDlgItemInt(
        dlg_.getWindowHandle(),
        ctrlId_,   // control identifier
        &translated, // success state
        false        // not signed
    );

    if (not translated)
        return initialVal_;

    return res;
}


void C::setValue(int val)
{
    if (not ctrl_)
    {
        initialVal_ = val;
        return;
    }

    if (val < min_)
        val = min_;
    else if (val > max_)
        val = max_;

    BOOL const res = ::SetDlgItemInt(
        dlg_.getWindowHandle(), // HWND hDlg,
        ctrlId_,                // int nIDDlgItem,
        (UINT)val,                // UINT uValue,
        true                      // BOOL bSigned
    );

    D1_ASSERT(res);
}


void C::enable(bool enable)
{
    ::EnableWindow(upDown_, enable);
    ::EnableWindow(ctrl_, enable);

    D1_VERIFY(::InvalidateRect(upDown_, 0, true));

    /*
    RECT r{};

    ::GetClientRect(upDown_, &r);

    D1_VERIFY(::InvalidateRect(upDown_, &r, false));
    */
}

}
