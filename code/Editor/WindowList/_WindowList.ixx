/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.WindowList;

import App.IApplication;

import d1.wintypes;

import WinUtil.IWindow;
import WinUtil.ProcRegistrar;
import WinUtil.Messages;

import std;


namespace Editor
{

export class WindowList
{
public:
    WindowList(
        App::IApplication& app,
        WinUtil::Window& window);

    WindowList(const WindowList&) = delete;
    WindowList& operator=(const WindowList&) = delete;

    ~WindowList(); // intentionally not virtual

    void setMenuEntry(const std::wstring& s);
    const std::wstring& getMenuEntry() const { return menuEntry_; }

    void needsUpdate();

private:
    using This = WindowList;

    App::IApplication& application_;
    WinUtil::Window& window_;
    std::wstring menuEntry_;
    bool needsUpdate_ = true;
    d1::HMENU windowMenu_ = {};
    d1::UINT insertCount_ = 0;

    WinUtil::ProcRegistrar procReg_;

    void onInitMenuPopup(WinUtil::WM_INITMENUPOPUP_Msg);
    void onCommand(WinUtil::WM_COMMAND_Msg);

    void activateWindow() const;
    void showDialog();
};

}
