/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module App.Control;

import App.Dialog;
import App.IApplication;

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

}
