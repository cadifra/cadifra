/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.Print;

import App.IApplication;

import Canvas.ICanvas;

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


namespace PrintAbortDialog
{

void showDialog(
    d1::HDC printerDC,
    App::IApplication& app,
    d1::HWND owner,
    const std::wstring& diagramName);
// Shows the print abort dialog. Has no effect if a print abort dialog exists
// yet. Installs a print abort callback function.

void hideDialog(d1::HDC printerDC);
// Hides the print abort dialog if there is one and removes the print abort
// callback function.

class Dlg
{
    d1::HDC DC_;

public:
    Dlg(
        d1::HDC printerDC,
        App::IApplication& app,
        d1::HWND owner,
        const std::wstring& diagramName):

        DC_{ printerDC }
    {
        showDialog(printerDC, app, owner, diagramName);
    }

    ~Dlg()
    {
        hideDialog(DC_);
    }
};

auto getAbort() -> const Canvas::IAbort&;

}

}
