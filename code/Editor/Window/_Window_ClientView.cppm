/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "d1/d1verify.h"

module Editor.Window:ClientView;

import :Window;

namespace Editor
{

class Window::ClientView:
    private Canvas::IScrollChange,
    private Canvas::IZoomChange
{
public:
    WinUtil::Window window_;
    HWND vScrollBar_ = {};
    std::unique_ptr<Canvas::IScreenCanvas> screenCanvas_;
    std::unique_ptr<View::DiagramView> diagramView_;
    Window& parent_;
    ClientView* opposite_;

    d1::Observer::C<Canvas::IScrollChange> scrollConnector_;
    d1::Observer::C<Canvas::IZoomChange> zoomConnector_;

    ClientView(Window& parent, ClientView* opposite);
    ~ClientView();

    //-- Canvas::IScrollChange

    void scrollNotification(const d1::fPoint&) override;

    //-- Canvas::IZoomChanged

    void zoomNotification(const double& zoomFactor,
        const d1::Point& fixPoint) override;

    //--
};

}


import :SplitBar;

import Editor.CmdHandlerDockCreator;

import Core.Main;

import View.Base;

import WinUtil.WindowClass;

import ScreenCanvas;


namespace Editor
{

Window::ClientView::ClientView(Window& parent, Window::ClientView* opposite):
    parent_{ parent },
    opposite_{ opposite }
{
    const HINSTANCE instance = parent.getApplication().getInstanceHandle();

    static auto clientClass = WinUtil::WindowClass{
        L"Editor::ClientView",       // lpszClassName
        &WinUtil::windowStartupProc, // lpfnWndProc
        instance,                    // hInstance

        CS_OWNDC |      // own device context
            CS_DBLCLKS, // Sends a double-click message to the window procedure
                        // when the user double-clicks the mouse while the cursor
                        // is within a window belonging to the class

        0, // HBRUSH    hbrBackground
        0, // HCURSOR   hCursor
        0, // lpszMenuName,
        0  // hIcon
    };

    HWND clientWindow = ::CreateWindow(
        clientClass.getAtom(),
        0,
        WS_VISIBLE | WS_CHILD,
        0, 0, 0, 0, // x, y, nWidth, nHeight
        parent.getWindowHandle(),
        0,
        instance,
        &window_);
    D1_ASSERT(clientWindow);

    vScrollBar_ = ::CreateWindow(
        L"SCROLLBAR",
        0,
        WS_VISIBLE | WS_CHILD | SBS_VERT,
        0, 0, 0, 0, // x, y, nWidth, nHeight
        parent.getWindowHandle(),
        0,
        instance,
        0);
    D1_ASSERT(vScrollBar_);

    screenCanvas_ = ScreenCanvas::create(
        window_, parent.HScrollBar_, vScrollBar_,
        parent.getApplication().getMessageLoop());

    auto dvp = View::DiagramViewParams{
        .diagram = parent.diagram_,
        .canvas = *screenCanvas_,
        .commander = parent.commander_.get(),
        .document = parent.document_,
        .window = window_,
        .clipboard = parent.clipboard_,
        .clipboard_viewer = *parent.clipboardViewer_,
        .dispatcher = window_.getDispatcher(),
        .mouse_input_control = parent,
        .transfer_set_format_handler = *(parent.transferSetFormatHandler_),
        .cmd_handler_dock_creator = CmdHandlerDockCreator::instance(),
        .diagram_type_observer = parent,
        .document_window = parent
    };

    diagramView_ = std::make_unique<View::DiagramView>(dvp);

    parent.diagram_.add(*diagramView_);

    diagramView_->update();
    screenCanvas_->resetViewPoint();

    if (opposite_)
    {
        screenCanvas_->copyZoomFactor(*opposite_->screenCanvas_);

        auto p = opposite_->screenCanvas_->clientToLogic(
            { 0, parent.splitBar_->splitPos() });

        screenCanvas_->scrollTo(p, {});

        auto sc = Core::Selection::Tracker{ diagramView_.get() };
        diagramView_->setSelection(
            sc, opposite_->diagramView_->selection());

        opposite_->opposite_ = this;
    }

    scrollConnector_ = screenCanvas_->getScrollConnector();
    scrollConnector_.connect(*this);

    zoomConnector_ = screenCanvas_->getZoomConnector();
    zoomConnector_.connect(*this);
}


Window::ClientView::~ClientView()
{
    if (opposite_)
        opposite_->opposite_ = 0;

    parent_.diagram_.forget(*diagramView_);
    D1_VERIFY(::DestroyWindow(vScrollBar_));

    if (parent_.activeClientView_ == this)
        parent_.activeClientView_ = 0;
}


void Window::ClientView::scrollNotification(const d1::fPoint&)
{
    if (not opposite_)
        return;

    auto thisPoint = screenCanvas_->clientToLogic({});
    auto otherPoint =
        opposite_->screenCanvas_->clientToLogic({});

    auto p = d1::point(thisPoint.x, otherPoint.y);

    opposite_->screenCanvas_->scrollTo(p, {});
}


void Window::ClientView::zoomNotification(
    const double& zoomFactor, const d1::Point& fixPoint)
{
    parent_.updateToZoomFactor();

    if (not opposite_)
        return;

    RECT c;
    D1_VERIFY(::GetClientRect(opposite_->window_.getWindowHandle(), &c));

    auto newFixPoint = d1::point(fixPoint.x, c.bottom / 2);

    opposite_->screenCanvas_->copyZoomFactor(*screenCanvas_, newFixPoint);
}

}
