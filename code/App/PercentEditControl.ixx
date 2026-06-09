/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <commctrl.h>

export module App.PercentEditControl;

import App.IApplication;
import App.Dialog;
import App.EditCtrlHelper;

import d1.types;
import d1.wintypes;

import WinUtil.ProcRegistrar;
import WinUtil.Messages;


namespace App
{

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

}


module : private;

import d1.buffer;
import d1.Range;
import d1.round;

import std;


namespace App
{

using C = PercentEditCtrl;


C::PercentEditCtrl(
    IApplication& app,
    Dialog& dlg,
    int control,
    const d1::float64& minValue,
    const d1::float64& maxValue,
    const d1::float64& initialValue,
    const d1::float64& deltaValue,
    IObserver* observer):

    procReg_{ dlg.getDispatcher(), 0 },
    editCtrlHelper_{ observer },
    app_{ app },
    dlg_{ dlg },
    ctrlId_{ control },
    min_{ minValue },
    max_{ maxValue },
    delta_{ deltaValue },
    lastValidVal_{ initialValue }
{
    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onInitDialog);
    ph.addSpy(&This::onCommand);
    ph.addSpy(&This::onNotify);
}


void C::onInitDialog(WinUtil::WM_INITDIALOG_Msg)
{
    auto sn = EditCtrlHelper<This>::SuppressNotifications(editCtrlHelper_);

    ctrl_ = ::GetDlgItem(dlg_.getWindowHandle(), ctrlId_);
    D1_ASSERT(ctrl_);

    upDown_ = ::CreateUpDownControl(
        WS_GROUP | WS_CHILD | WS_BORDER | WS_VISIBLE |
            UDS_ALIGNRIGHT | UDS_ARROWKEYS,
        0, 0, 0, 0,
        dlg_.getWindowHandle(),
        -1,
        app_.getInstanceHandle(),
        ctrl_,
        1, 0, 0);
    D1_ASSERT(upDown_);

    ::SendMessage(ctrl_, EM_LIMITTEXT, 30, 0);

    internalSetValue(lastValidVal_);
}


void C::onCommand(WinUtil::WM_COMMAND_Msg msg)
{
    if (msg.hwndCtl() != ctrl_)
        return;

    if (msg.wNotifyCode() == EN_CHANGE)
    {
        editCtrlHelper_.notifyObserver(*this);
    }
    else if (msg.wNotifyCode() == EN_KILLFOCUS)
    {
        switch (getState())
        {
            using enum State;
        case VALID:
            setValue(getValue());
            break; // no notification
        case TOBIG:
            internalSetValue(max_);
            break;
        case TOSMALL:
            internalSetValue(min_);
            break;
        default:
            internalSetValue(lastValidVal_);
            break;
        }
    }
}


auto C::getState() const -> State
{
    auto s = getString();

    using enum State;

    if (not s.size())
        return INVALIDNUMBER;

    auto ss = std::wistringstream{ s };

    d1::float64 f{};
    std::wstring symbol;

    ss >> f;

    if (not ss)
        return INVALIDNUMBER;

    ss >> symbol;

    if (symbol.size() and symbol != L"%")
        return INVALIDNUMBER;

    if (f < min_)
        return TOSMALL;

    if (f > max_)
        return TOBIG;

    return VALID;
}


d1::float64 C::getValue()
{
    switch (getState())
    {
        using enum State;
    case INVALIDNUMBER:
        return lastValidVal_;
    case TOBIG:
        lastValidVal_ = max_;
        return max_;
    case TOSMALL:
        lastValidVal_ = min_;
        return min_;
    }

    auto s = getString();

    auto ss = std::wistringstream{ s };

    ss >> lastValidVal_;

    return lastValidVal_;
}


void C::internalSetValue(const d1::float64& f)
{
    auto o = std::wostringstream{};
    o << f << L" %";
    setString(o.str());
}


void C::setValue(const d1::float64& f)
{
    if (editCtrlHelper_.isNotifying())
        return;

    auto sn = EditCtrlHelper<This>::SuppressNotifications{ editCtrlHelper_ };

    if (not ctrl_)
    {
        lastValidVal_ = f;
        return;
    }

    internalSetValue(f);
}


void C::onNotify(WinUtil::WM_NOTIFY_Msg msg)
{
    if (msg.pnmh()->hwndFrom == upDown_ and msg.pnmh()->code == UDN_DELTAPOS)
    {
        LPNMUPDOWN m = reinterpret_cast<LPNMUPDOWN>(msg.getLParam());

        int f = d1::round(getValue() / delta_);

        d1::float64 r = (f + m->iDelta) * delta_;

        d1::range(min_, max_).limit(r);

        internalSetValue(r);

        ::SetWindowLongPtr(dlg_.getWindowHandle(), DWLP_MSGRESULT, TRUE);
        // prohibit a change of the scroll position
    }
}


std::wstring C::getString() const
{
    if (not ctrl_)
        return L"";

    auto len = static_cast<int>(::SendMessage(ctrl_, EM_LINELENGTH, 0, 0));

    if (len <= 0)
        return L"";

    auto s = d1::wbuffer(len + 2);

    auto* buf = const_cast<wchar_t*>(s.c_str());

    *reinterpret_cast<WORD*>(buf) = static_cast<WORD>(s.capacity());

    len = static_cast<int>(::SendMessage(
        ctrl_, EM_GETLINE, 0, reinterpret_cast<LPARAM>(buf)));

    s.resize(len);

    return s;
}


void C::setString(const std::wstring& s) const
{
    if (not ctrl_)
        return;

    D1_VERIFY(TRUE == ::SendMessage(
                          ctrl_, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(s.c_str())));
}

}
