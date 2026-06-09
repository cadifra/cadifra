/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.IWindow;

import App.IWindow;
import App.ICmdView;

import Canvas.ScreenCanvas;

import Core.Main;

import Style.IElementStyleRegistry;

import View.DiagramView;
import View.DiagramType;


namespace Editor
{

export class IWindow:
    public App::IWindow,
    public Style::IElementStyleRegistryOwner
{
public:
    using App::IWindow::IWindow;

    virtual auto getScreenCanvas() const -> Canvas::IScreenCanvas& = 0;

    virtual auto getActiveDiagramView() const -> View::DiagramView* = 0;

    virtual void add(const std::shared_ptr<App::ICmdView>&) = 0;

    virtual auto getDiagramType() const -> View::DiagramTypeValue::T = 0;

    virtual auto getDiagram() -> Core::IDiagram& = 0;

protected:
    ~IWindow() = default;
};

}
