/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module Editor.ZoomDialog;

import App.ModelessDialog;
import App.ExecRegistrar;
import App.PercentEditControl;
import App.DialogLayout;

import Canvas.ScreenCanvas;

import WinUtil.WindowPlacement;


namespace Editor
{

export class IZoomDialogOwner
{
public:
    virtual void updateToFactor() = 0;
    virtual void dialogClosed() = 0;

protected:
    ~IZoomDialogOwner() = default;
};


export class ZoomDialog:
    public App::ModelessDialog,
    private App::PercentEditCtrl::IObserver
{
    using This = ZoomDialog;

    IZoomDialogOwner& owner_;

    WinUtil::WindowPlacementHandler& wp_;

    WinUtil::ProcRegistrar pr_;
    App::ExecRegistrar er_;
    Canvas::IScreenCanvas& sc_;
    HWND window_;

    App::PercentEditCtrl percentCtrl_; // keep this at last pos!!

public:
    ZoomDialog(
        IZoomDialogOwner& owner,
        App::IApplication&, Canvas::IScreenCanvas& sc, HWND w,
        WinUtil::WindowPlacementHandler&);

    void closing();

    void updateToFactor();

    void toForeground();

private:
    void changeNotification(App::PercentEditCtrl&);

    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
    void onOK(const WinUtil::WM_COMMAND_Msg&);

    void onZoom400(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(400); }
    void onZoom200(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(200); }
    void onZoom150(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(150); }
    void onZoom100(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(100); }
    void onZoom75(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(75); }
    void onZoom50(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(50); }
    void onZoom25(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(25); }

    void onFitWindow(const WinUtil::WM_COMMAND_Msg&);

    void onClose(WinUtil::WM_CLOSE_Msg);

    void onActivate(WinUtil::WM_ACTIVATE_Msg);

    void setRadioZoom(int percent);

    void updateButtons();

    void updateRadioButtons();
    void uncheckAllRadioButtons();

    int tuned(int percent) const;

    int mappedZoomFactor(int percent) const;

    void setFocusAndSelect(int edit_ctrl_id);

    void savePlacement();
    void restorePlacement();
};

}

module : private;

import d1.Point;
import d1.round;


namespace Editor
{

constexpr d1::float64
    MinPercent = 5,
    MaxPercent = 5000;


inline d1::Point center(HWND hwnd)
{
    RECT r;
    D1_VERIFY(::GetClientRect(hwnd, &r));
    return { r.right / 2, r.bottom / 2 };
}


enum ResourceConstants
{
    PercentId = edt1,

    RadioFirstId = rad1,
    Zoom400Id = rad1,
    Zoom200Id = rad2,
    Zoom150Id = rad3,
    Zoom100Id = rad4,
    Zoom75Id = rad5,
    Zoom50Id = rad6,
    Zoom25Id = rad7,
    RadioLastId = rad7,

    FitWindowId = psh1
};


using C = ZoomDialog;


void C::onInitDialog(WinUtil::WM_INITDIALOG_Msg)
{
    restorePlacement();
    updateButtons();
}


void C::onOK(const WinUtil::WM_COMMAND_Msg&)
{
    closing();
    owner_.dialogClosed();
}


void C::onFitWindow(const WinUtil::WM_COMMAND_Msg&)
{
    sc_.zoomToFit();
    const int p = d1::round(sc_.getZoomFactor() * 100.0);
    percentCtrl_.setValue(p);
    updateButtons();
    setFocusAndSelect(PercentId);
}


void C::setRadioZoom(int percent)
{
    sc_.setZoomFactor(percent / 100.0, center(window_));
    percentCtrl_.setValue(percent);
    updateButtons();
}


void C::updateButtons()
{
    updateRadioButtons();
}


int C::tuned(int percent) const
{
    return d1::round(sc_.calculateZoomFactor(percent / 100.0) * 100.0);
}


void C::setFocusAndSelect(int edit_ctrl_id)
{
    const HWND h = ::GetDlgItem(getWindowHandle(), edit_ctrl_id);
    ::SetFocus(h);
    ::SendMessage(h, EM_SETSEL, 0, -1);
}


C::ZoomDialog(
    IZoomDialogOwner& owner,
    App::IApplication& app,
    Canvas::IScreenCanvas& sc,
    HWND w,
    WinUtil::WindowPlacementHandler& wp):

    ModelessDialog{ app },
    owner_{ owner },
    wp_{ wp },
    pr_{ getDispatcher(), 0 },
    sc_{ sc },
    er_{ getCommander(), 0 },
    window_{ w },

    percentCtrl_(
        app, *this, PercentId, MinPercent, MaxPercent,
        mappedZoomFactor(d1::round(sc.getZoomFactor() * 100.0)),
        5.0, this)
{
    auto prh = pr_.helper(*this);
    auto erh = er_.helper(*this);

    prh.addSpy(&This::onInitDialog);
    prh.addAlwaysReady(&This::onClose);
    prh.addAlwaysReady(&This::onActivate);

    erh.add(IDOK, &This::onOK);
    erh.add(FitWindowId, &This::onFitWindow);

    erh.add(Zoom400Id, &This::onZoom400);
    erh.add(Zoom200Id, &This::onZoom200);
    erh.add(Zoom150Id, &This::onZoom150);
    erh.add(Zoom100Id, &This::onZoom100);
    erh.add(Zoom75Id, &This::onZoom75);
    erh.add(Zoom50Id, &This::onZoom50);
    erh.add(Zoom25Id, &This::onZoom25);
}


void C::changeNotification(App::PercentEditCtrl& p)
{
    if (&p == &percentCtrl_)
    {
        const int percent = d1::round(percentCtrl_.getValue());
        sc_.setZoomFactor(percent / 100.0, center(window_));
        updateButtons();
    }
}


void C::updateRadioButtons()
{
    const int p = d1::round(percentCtrl_.getValue());

    int id = 0;
    if (p == 400)
        id = Zoom400Id;
    else if (p == 200)
        id = Zoom200Id;
    else if (p == 150)
        id = Zoom150Id;
    else if (p == 100)
        id = Zoom100Id;
    else if (p == 75)
        id = Zoom75Id;
    else if (p == 50)
        id = Zoom50Id;
    else if (p == 25)
        id = Zoom25Id;

    if (id == 0)
    {
        uncheckAllRadioButtons();
        return;
    }

    BOOL res = ::CheckRadioButton(
        getWindowHandle(), RadioFirstId, RadioLastId, id);
}


int C::mappedZoomFactor(int p) const
{
    int ep = 0;

    if (p == tuned(400))
        ep = 400;
    else if (p == tuned(200))
        ep = 200;
    else if (p == tuned(150))
        ep = 150;
    else if (p == tuned(100))
        ep = 100;
    else if (p == tuned(75))
        ep = 75;
    else if (p == tuned(50))
        ep = 50;
    else if (p == tuned(25))
        ep = 25;

    return (ep ? ep : p);
}


void C::uncheckAllRadioButtons()
{
    for (int id = RadioFirstId; id != RadioLastId + 1; ++id)
    {
        BOOL res = ::CheckDlgButton(getWindowHandle(), id, BST_UNCHECKED);
    }
}


void C::onActivate(WinUtil::WM_ACTIVATE_Msg)
{
    // Prevent the dialog code from saving and restoring the focus. The first
    // control in the dialog will get the focus automatically.
}


void C::toForeground()
{
    ::SetForegroundWindow(getWindowHandle());
}


void C::closing()
{
    savePlacement();
}


void C::onClose(WinUtil::WM_CLOSE_Msg)
{
    closing();
    owner_.dialogClosed();
}


void C::savePlacement()
{
    wp_.getFrom(getWindowHandle());
}


void C::restorePlacement()
{
    namespace DL = App::DialogLayout;

    const HWND w = getWindowHandle();

    if (not wp_.ok())
    {
        DL::defaultPlace(w);
        return;
    }

    wp_.setTo(w);
}


void C::updateToFactor()
{
    const int p = mappedZoomFactor(d1::round(sc_.getZoomFactor() * 100.0));
    percentCtrl_.setValue(p);

    updateButtons();
}

}
