/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include "Resources/CommandMacro.h"

#include <Windows.h>

export module Editor.ZoomCmdHandler;

import Editor.IWindow;
import Editor.ZoomDialog;

import App.Command;

import std;


namespace Editor
{

export class ZoomCmdHandler: public IZoomDialogOwner
{
    class Rep;
    std::unique_ptr<Rep> rep;

public:
    void updateToFactor() override;
    void dialogClosed() override;

    ZoomCmdHandler(const IWindow& parent, App::ICommander&);

    ZoomCmdHandler(const ZoomCmdHandler&) = delete;
    ZoomCmdHandler& operator=(const ZoomCmdHandler&) = delete;

    ~ZoomCmdHandler();
};

}

module : private;

import App.IApplication;
import App.ExecRegistrar;

import Canvas.ScreenCanvas;

import d1.Point;
import d1.round;

import WinUtil.Messages;
import WinUtil.WindowPlacement;

import Resources;


D1_RESOURCES_USE_COMMAND(Zoom200)
D1_RESOURCES_USE_COMMAND(Zoom100)
D1_RESOURCES_USE_COMMAND(Zoom50)
D1_RESOURCES_USE_COMMAND(Zoom25)
D1_RESOURCES_USE_COMMAND(ZoomCustom)
D1_RESOURCES_USE_COMMAND(ZoomFit)
D1_RESOURCES_USE_COMMAND(ZoomSelection)
D1_RESOURCES_USE_COMMAND(ZoomIn)
D1_RESOURCES_USE_COMMAND(ZoomOut)


namespace Editor
{

using C = ZoomCmdHandler;


class C::Rep
{
    using This = Rep;

    ZoomCmdHandler& owner_;
    const IWindow& parent_;
    App::ExecRegistrar exreg_;
    WinUtil::WindowPlacementHandler wp_;
    std::unique_ptr<ZoomDialog> dialog_;

public:
    Rep(ZoomCmdHandler& owner, const IWindow& p, App::ICommander&);

    void updateToFactor();

    d1::Point center() const;
    void setFactor(int percent);

    HWND getWindowHandle() const { return parent_.getWindowHandle(); }
    auto getScreenCanvas() const -> Canvas::IScreenCanvas&
    {
        return parent_.getScreenCanvas();
    }
    auto getApplication() const -> App::IApplication&
    {
        return parent_.getApplication();
    }

    using CMD_Msg = WinUtil::WM_COMMAND_Msg;

    void cmdZoom200(const CMD_Msg&);
    void cmdZoom100(const CMD_Msg&);
    void cmdZoom50(const CMD_Msg&);
    void cmdZoom25(const CMD_Msg&);
    void cmdZoomCustom(const CMD_Msg&);
    void cmdZoomFit(const CMD_Msg&);
    void cmdZoomIn(const CMD_Msg&);
    void cmdZoomOut(const CMD_Msg&);

    void dialogClosed();
};


C::Rep::Rep(ZoomCmdHandler& owner, const IWindow& p, App::ICommander& c):

    owner_{ owner },
    parent_{ p },
    exreg_{ c, 0 }
{
    auto eh = exreg_.helper(*this);

    namespace C = Resources::Commands;

    eh.add(C::Zoom200, &This::cmdZoom200);
    eh.add(C::Zoom100, &This::cmdZoom100);
    eh.add(C::Zoom50, &This::cmdZoom50);
    eh.add(C::Zoom25, &This::cmdZoom25);
    eh.add(C::ZoomFit, &This::cmdZoomFit);
    eh.add(C::ZoomIn, &This::cmdZoomIn);
    eh.add(C::ZoomOut, &This::cmdZoomOut);
    eh.add(C::ZoomCustom, &This::cmdZoomCustom);
}


C::ZoomCmdHandler(const IWindow& p, App::ICommander& c):

    rep{ std::make_unique<Rep>(*this, p, c) }
{
}


C::~ZoomCmdHandler()
{
}


d1::Point C::Rep::center() const
{
    RECT r;
    D1_VERIFY(::GetClientRect(getWindowHandle(), &r));
    return { r.right / 2, r.bottom / 2 };
}


void C::Rep::setFactor(int percent)
{
    getScreenCanvas().setZoomFactor(percent / 100.0, center());
}


void C::Rep::cmdZoom200(const CMD_Msg&) { setFactor(200); }
void C::Rep::cmdZoom100(const CMD_Msg&) { setFactor(100); }
void C::Rep::cmdZoom50(const CMD_Msg&) { setFactor(50); }
void C::Rep::cmdZoom25(const CMD_Msg&) { setFactor(25); }


void C::Rep::cmdZoomCustom(const CMD_Msg&)
{
    if (dialog_)
    {
        dialog_->toForeground();
        return;
    }

    dialog_ = std::make_unique<ZoomDialog>(
        owner_, getApplication(), getScreenCanvas(), getWindowHandle(), wp_);

    dialog_->create(getWindowHandle(), L"ZOOM");
}


void C::Rep::cmdZoomFit(const CMD_Msg&)
{
    getScreenCanvas().zoomToFit();
}


void C::Rep::cmdZoomIn(const CMD_Msg&)
{
    getScreenCanvas().incrementZoom(center());
}


void C::Rep::cmdZoomOut(const CMD_Msg&)
{
    getScreenCanvas().decrementZoom(center());
}


int tuned(const Canvas::IScreenCanvas& sc, int percent)
{
    return d1::round(sc.calculateZoomFactor(percent / 100.0) * 100.0);
}


void C::Rep::updateToFactor()
{
    auto& sc = parent_.getScreenCanvas();

    const int p = d1::round(sc.getZoomFactor() * 100.0);

    namespace C = Resources::Commands;
    auto& er = exreg_;

    using T = App::CmdState::T;
    constexpr auto uc = T::unchecked;
    constexpr auto rc = T::radio_checked;

    er.setState(C::Zoom200, uc);
    er.setState(C::Zoom100, uc);
    er.setState(C::Zoom50, uc);
    er.setState(C::Zoom25, uc);
    er.setState(C::ZoomCustom, uc);

    if (p == tuned(sc, 200))
        er.setState(C::Zoom200, rc);
    else if (p == tuned(sc, 100))
        er.setState(C::Zoom100, rc);
    else if (p == tuned(sc, 50))
        er.setState(C::Zoom50, rc);
    else if (p == tuned(sc, 25))
        er.setState(C::Zoom25, rc);
    else
        er.setState(C::ZoomCustom, rc);

    if (dialog_)
        dialog_->updateToFactor();
}


void C::Rep::dialogClosed()
{
    dialog_.reset();
}


void C::updateToFactor()
{
    rep->updateToFactor();
}


void C::dialogClosed()
{
    rep->dialogClosed();
}

}
