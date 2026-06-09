/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <dlgs.h>

export module App.ErrorSaveAsDialog;

import App.ModalDialog;

import d1.NoValueSemantics;

import std;


namespace App
{

export class SaveAsHandler: public d1::NoValueSemantics
{
public:
    virtual void saveAs(HWND owner) = 0;

protected:
    ~SaveAsHandler() = default;
};


export class ErrorSaveAsDialog: public ModalDialog
{
    using This = ErrorSaveAsDialog;

public:
    ErrorSaveAsDialog(
        SaveAsHandler&,
        const std::wstring& text);

private:
    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
    void onSaveAs(const WinUtil::WM_COMMAND_Msg&);

    WinUtil::ProcRegistrar pr_;
    ExecRegistrar er_;
    SaveAsHandler& sah_;
    std::wstring text_;
};

}

module : private;

import App.DialogLayout;


namespace App
{

namespace
{

enum ResourceConstants
{
    SaveAsId = psh1,
    TextId = stc1
};

using C = ErrorSaveAsDialog;
}

void C::onInitDialog(WinUtil::WM_INITDIALOG_Msg)
{
    ::SetDlgItemText(
        getWindowHandle(),
        TextId,
        text_.c_str());

    DialogLayout::defaultPlace(getWindowHandle());
}


void C::onSaveAs(const WinUtil::WM_COMMAND_Msg&)
{
    sah_.saveAs(getWindowHandle());
}


C::ErrorSaveAsDialog(
    SaveAsHandler& sah,
    const std::wstring& text):

    pr_{ getDispatcher(), 0 },
    er_{ getCommander(), 0 },
    sah_{ sah },
    text_(text)
{
    auto prh = pr_.helper(*this);
    auto erh = er_.helper(*this);

    prh.addSpy(&This::onInitDialog);

    erh.add(SaveAsId, &This::onSaveAs);
}

}
