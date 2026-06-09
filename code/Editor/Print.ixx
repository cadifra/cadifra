/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.Print;

import App.IApplication;

import Canvas.PrinterCanvas;

import d1.wintypes;

import Core.Main;

import std;


namespace Editor
{

export void printDiagram(
    App::IApplication& app,
    Core::IDiagram& d,
    const std::wstring& name,
    d1::HWND ownerwindow);


// Print without user invention:

export void printDiagram(
    App::IApplication& app,
    Core::IDiagram& d,
    const std::wstring& name,
    d1::HWND ownerwindow,
    const std::wstring& printer,
    const std::wstring& driver,
    const std::wstring& output);

}
