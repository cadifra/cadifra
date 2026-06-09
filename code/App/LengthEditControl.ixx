/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <commctrl.h>

export module App.LengthEditControl;

import App.Dialog;
import App.IApplication;
import App.EditCtrlHelper;

import d1.StackFlag;
import d1.types;
import d1.wintypes;

import WinUtil.Messages;
import WinUtil.ProcRegistrar;

import std;


namespace App
{

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

}

module : private;


import d1.buffer;
import d1.Range;
import d1.round;
import d1.types;

import std;


namespace App
{

namespace
{
using C = LengthEditCtrl;
}


void C::init()
{
    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onInitDialog);
    ph.addSpy(&This::onCommand);
    ph.addSpy(&This::onNotify);

    displayUnits_ = getUserSettingUnits();
}


void C::onInitDialog(WinUtil::WM_INITDIALOG_Msg)
{
    ctrl_ = ::GetDlgItem(dlg_.getWindowHandle(), ctrlId_);
    D1_ASSERT(ctrl_);

    upDown_ = ::CreateUpDownControl(
        WS_CHILD | WS_BORDER | WS_VISIBLE |
            UDS_ALIGNRIGHT | UDS_ARROWKEYS,
        0, 0, 0, 0,
        dlg_.getWindowHandle(),
        -1,
        app_.getInstanceHandle(),
        ctrl_,
        1, 0, 0);
    D1_ASSERT(upDown_);

    ::SendMessage(ctrl_, EM_LIMITTEXT, 30, 0);

    showValue();
}


void C::onCommand(WinUtil::WM_COMMAND_Msg msg)
{
    if (msg.hwndCtl() != ctrl_)
        return;


    if (msg.wNotifyCode() == EN_CHANGE)
    {
        if (ignoreFlag_)
            return;

        evaluate();
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
            internalSetValue(value_);
            break;
        }
    }
}


void C::settingChanged()
{
    displayUnits_ = getUserSettingUnits();
    showValue();
}


auto C::getState() const -> State
{
    return state_;
}


d1::float64 C::getValue() const
{
    auto r = value_;
    d1::range(min_, max_).limit(r);
    return r;
}


void C::onNotify(WinUtil::WM_NOTIFY_Msg msg)
{
    if (msg.pnmh()->hwndFrom == upDown_ and msg.pnmh()->code == UDN_DELTAPOS)
    {
        auto m = reinterpret_cast<LPNMUPDOWN>(msg.getLParam());

        d1::float64 v = convert(value_, MILLIMETER, displayUnits_);

        int f = d1::round(v / delta_);

        v = (f + m->iDelta) * delta_;

        d1::float64 r = convert(v, displayUnits_, MILLIMETER);

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

    *reinterpret_cast<WORD*>(buf) = (WORD)s.capacity();

    len = static_cast<int>(
        ::SendMessage(ctrl_, EM_GETLINE, 0, reinterpret_cast<LPARAM>(buf)));

    s.resize(len);

    return s;
}


void C::setString(const std::wstring& s)
{
    if (not ctrl_)
        return;

    auto stackFlag = d1::StackFlag{ ignoreFlag_ };

    D1_VERIFY(TRUE == ::SendMessage(
                          ctrl_, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(s.c_str())));
}


void C::evaluate()
{
    auto s = getString();

    if (not s.size())
    {
        state_ = State::INVALIDNUMBER;
        return;
    }

    auto ss = std::wistringstream(s);
    ss.imbue(std::locale(""));

    d1::float64 f{};
    ss >> f;

    if (not ss)
    {
        state_ = State::INVALIDNUMBER;
        return;
    }

    std::wstring units;
    ss >> units;

    displayUnits_ = getUserSettingUnits();

    if (not units.size())
        ; // do nothing
    else if (units == L"\"" or units == L"in" or units == L"inch" or units == L"inches")
        displayUnits_ = INCH;
    else if (units == L"\'" or units == L"ft" or units == L"foot" or units == L"feet")
        displayUnits_ = FOOT;
    else if (units == L"m")
        displayUnits_ = METER;
    else if (units == L"cm")
        displayUnits_ = CENTIMETER;
    else if (units == L"mm")
        displayUnits_ = MILLIMETER;
    else
    {
        state_ = State::INVALIDUNITS;
        return;
    }

    value_ = convert(f, displayUnits_, MILLIMETER);

    if (value_ < min_)
        state_ = State::TOSMALL;
    else if (value_ > max_)
        state_ = State::TOBIG;
    else
        state_ = State::VALID;
}


void C::internalSetValue(const d1::float64& f)
{
    value_ = f;
    d1::range(min_, max_).limit(value_);
    state_ = State::VALID;
    showValue();
    editCtrlHelper_.notifyObserver(*this);
}


void C::setValue(const d1::float64& f)
{
    if (editCtrlHelper_.isNotifying())
        return;

    auto sn = EditCtrlHelper<This>::SuppressNotifications{ editCtrlHelper_ };
    internalSetValue(f);
}


void C::showValue()
{
    auto o = std::wostringstream{};
    o.imbue(std::locale(""));

    o << std::setprecision(precision_) << convert(value_, MILLIMETER, displayUnits_);

    switch (displayUnits_)
    {
    default: // fall through
    case INCH:
        o << L"\"";
        break;
    case FOOT:
        o << L"\'";
        break;
    case METER:
        o << L"m";
        break;
    case CENTIMETER:
        o << L"cm";
        break;
    case MILLIMETER:
        o << L"mm";
        break;
    }

    setString(o.str());
}


d1::float64 C::convert(const d1::float64& f, Units from, Units to)
{
    // convert anything to meters:

    if (from == to)
        return f;

    d1::float64 meter{};

    const d1::float64 INCH2METER = 25.4e-3;
    const d1::float64 FOOT2METER = 12 * INCH2METER;
    const d1::float64 CENTI = 1e-2;
    const d1::float64 MILLI = 1e-3;


    switch (from)
    {
    default: // fall through
    case INCH:
        meter = f * INCH2METER;
        break;
    case FOOT:
        meter = f * FOOT2METER;
        break;
    case METER:
        meter = f;
        break;
    case CENTIMETER:
        meter = f * CENTI;
        break;
    case MILLIMETER:
        meter = f * 1e-3;
        break;
    }

    switch (to)
    {
    default: // fall through
    case INCH:
        return meter / INCH2METER;
    case FOOT:
        return meter / FOOT2METER;
    case METER:
        return meter;
    case CENTIMETER:
        return meter / CENTI;
    case MILLIMETER:
        return meter / MILLI;
    }
}


auto C::getUserSettingUnits() const -> Units
{
    // see LOCALE_IMEASURE documentation for the magic numbers
    TCHAR buf[2];
    if (2 != ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, buf, 2))
        return USsystemUnits_;

    if (buf[0] == TEXT('0'))
        return SIUnits_;
    else
        return USsystemUnits_;
}

}

import d1.ListSet;

import WinUtil.ListenerWindow;


namespace App
{

namespace
{

class Observer
{
    using This = Observer;
    using Set = d1::ListSet<App::ISettingChanged*>;
    Set set_;
    WinUtil::ListenerWindow listenerWindow_;
    WinUtil::ProcRegistrar procReg_;

    void onSettingChange(WinUtil::WM_SETTINGCHANGE_Msg msg);

public:
    Observer();
    static auto instance() -> Observer&; // singleton
    void insert(ISettingChanged& o);
    void remove(ISettingChanged& o);
};


Observer::Observer():
    procReg_{ listenerWindow_.getDispatcher(), 0 }
{
    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onSettingChange);
}


auto Observer::instance() -> Observer&
{
    static Observer i;
    return i;
}


void Observer::onSettingChange(WinUtil::WM_SETTINGCHANGE_Msg msg)
{
    for (auto* o : set_)
        o->settingChanged();
}


void Observer::insert(App::ISettingChanged& o)
{
    set_.insert(&o);
}


void Observer::remove(App::ISettingChanged& o)
{
    set_.erase(&o);
}

}


ISettingChanged::Bookkeeper::Bookkeeper(ISettingChanged& o):
    observer_(o)
{
    Observer::instance().insert(observer_);
}


ISettingChanged::Bookkeeper::~Bookkeeper()
{
    Observer::instance().remove(observer_);
}

}
