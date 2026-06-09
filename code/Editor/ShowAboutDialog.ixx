/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "main/build-config.h"
#include "main/parent-str.h"

#include "d1/d1verify.h"

#include <Windows.h>
#include <dlgs.h>

export module Editor.ShowAboutDialog;

import App.IApplication;

import d1.wintypes;


namespace Editor
{

export void showAboutDialog(App::IApplication&, d1::HWND parent);

}


module : private;


import App.ModalDialog;

import WinUtil.CursorManager;

import std;


namespace Editor
{

class AboutDialog: public App::ModalDialog
{
    using This = AboutDialog;

    WinUtil::ProcRegistrar pr_;
    App::ExecRegistrar er_;
    HWND window_;

public:
    AboutDialog(App::IApplication&, HWND w);

private:
    enum ResourceConstants
    {
        VersionId = stc1
    };


    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
};


AboutDialog::AboutDialog(
    App::IApplication& app,
    HWND w):

    pr_{ getDispatcher(), 0 },
    er_{ getCommander(), 0 },
    window_{ w }
{
    auto prh = pr_.helper(*this);

    prh.addSpy(&This::onInitDialog);
}


void AboutDialog::onInitDialog(WinUtil::WM_INITDIALOG_Msg)
{
    const HWND h = ::GetDlgItem(getWindowHandle(), VersionId);

    auto os = std::wostringstream{};
    os << "Version ";

    os << D1_MAIN_BUILD_CONFIG_PRODUCT_VERSION_FIRST << L'.'
       << D1_MAIN_BUILD_CONFIG_PRODUCT_VERSION_SECOND << L'.'
       << D1_MAIN_BUILD_CONFIG_PRODUCT_VERSION_THIRD;

    os << " (" << main::parent_str() << ")";

    BOOL res = ::SetWindowText(
        h,
        os.str().c_str());
}


void showAboutDialog(
    App::IApplication& app,
    HWND parent)
{
    auto immediateWaitCursor = WinUtil::CursorManager::ImmediateWaitCursor{};

    auto d = AboutDialog{ app, parent };

    if (IDOK != d.show(app, parent, L"ABOUT"))
        return;
}

}
