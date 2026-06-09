/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <dlgs.h>

export module Editor.PrintAbortDialog;

import App.IApplication;

import Canvas.PrinterCanvas;

import d1.wintypes;

import std;


namespace Editor::PrintAbortDialog
{

export void showDialog(
    d1::HDC printerDC,
    App::IApplication& app,
    d1::HWND owner,
    const std::wstring& diagramName);
// Shows the print abort dialog. Has no effect if a print abort dialog exists
// yet. Installs a print abort callback function.

export void hideDialog(d1::HDC printerDC);
// Hides the print abort dialog if there is one and removes the print abort
// callback function.

export class Dlg
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

export auto getAbort() -> const Canvas::IAbort&;

}

module : private;


import App.ModelessDialog;
import App.ExecRegistrar;

import d1.buffer;

import WinUtil.GuardedFunctionCall;


namespace Editor
{

struct Aborter: public Canvas::IAbort
{
    bool abort_ = true;

    bool abort() const { return abort_; }
    Aborter() {}

    static Aborter& instance()
    { // singleton
        static Aborter a;
        return a;
    }
};


class ADlg: public App::ModelessDialog
{
    using This = ADlg;

    HWND owner_;
    App::ExecRegistrar execReg_;

    void onCANCEL(const WinUtil::WM_COMMAND_Msg&);

public:
    ADlg(App::IApplication&, HWND owner, const std::wstring&);
    ~ADlg();
};

auto theDlg = std::unique_ptr<ADlg>{};


ADlg::ADlg(App::IApplication& app, HWND owner, const std::wstring& name):
    App::ModelessDialog{ app },
    owner_{ owner },
    execReg_{ getCommander(), 0 }
{
    auto eh = execReg_.helper(*this);

    eh.add(IDCANCEL, &This::onCANCEL);

    create(owner, L"PRINTABORT");

    // Initialize the text of the dialog box:

    auto buf = d1::wbuffer(200);
    buf.resize(
        ::GetDlgItemText(
            getWindowHandle(),
            stc1,
            buf.data(),
            static_cast<int>(buf.size())));

    buf.replace(buf.find(L"%1"), 2, name);

    D1_VERIFY(::SetDlgItemText(
        getWindowHandle(),
        stc1,
        buf.c_str()));

    // Calculate the position of the dialog box:
    // (centered in the owner window but inside the desktop window)

    RECT r;
    D1_VERIFY(::GetWindowRect(owner, &r));

    auto center = POINT{
        .x = (r.right + r.left) / 2,
        .y = (r.bottom + r.top) / 2
    };

    D1_VERIFY(::GetWindowRect(getWindowHandle(), &r));

    auto size = SIZE{
        .cx = r.right - r.left,
        .cy = r.bottom - r.top
    };

    auto pos = POINT{
        .x = center.x - size.cx / 2,
        .y = center.y - size.cy / 2
    };

    RECT desk;
    D1_VERIFY(::GetWindowRect(::GetDesktopWindow(), &desk));

    if ((pos.x + size.cx) > desk.right)
        pos.x = desk.right - size.cx;

    if (pos.x < desk.left)
        pos.x = desk.left;

    if ((pos.y + size.cy) > desk.bottom)
        pos.y = desk.bottom - size.cy;

    if (pos.y < desk.top)
        pos.y = desk.top;

    D1_VERIFY(::SetWindowPos(
        getWindowHandle(),
        0,
        pos.x,
        pos.y,
        0,
        0,
        SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW));

    // Inhibit any user input (like for a modal dialog box)

    ::EnableWindow(owner_, FALSE);
    D1_ASSERT(::GetLastError() == S_OK);

    ::UpdateWindow(getWindowHandle());
}


ADlg::~ADlg()
{
    D1_ASSERT((::SetLastError(S_OK), true));
    ::EnableWindow(owner_, TRUE);
    D1_ASSERT(::GetLastError() == S_OK);
}


void ADlg::onCANCEL(const WinUtil::WM_COMMAND_Msg&)
{
    Aborter::instance().abort_ = true;
}


BOOL CALLBACK abortProc(
    HDC hdc,   // handle to DC
    int iError // error value
)
{
    struct GFC: public WinUtil::GuardedFunctionCallRet<BOOL>
    {
        GFC():
            WinUtil::GuardedFunctionCallRet<BOOL>(
                "Editor/PrintAbortDialog.cpp/AbortProc", FALSE)
        {
        }

        BOOL implementCallRet()
        {
            if (Aborter::instance().abort())
                return FALSE;

            if (theDlg)
                theDlg->getApplication().getMessageLoop().processMessages();

            return TRUE;
        }
    } gfc;

    gfc.execute();
    return gfc.res();
}


namespace PrintAbortDialog
{

void showDialog(
    HDC printerDC,
    App::IApplication& app,
    HWND owner,
    const std::wstring& diagramName)
{
    Aborter::instance().abort_ = false;
    D1_VERIFY(0 < ::SetAbortProc(printerDC, abortProc));

    if (not theDlg)
        theDlg = std::make_unique<ADlg>(app, owner, diagramName);
}


void hideDialog(HDC printerDC)
{
    Aborter::instance().abort_ = true;
    theDlg = {};
}


auto getAbort() -> const Canvas::IAbort&
{
    return Aborter::instance();
}

}

}
