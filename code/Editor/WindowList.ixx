/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module Editor.WindowList;

import App.IApplication;

import d1.wintypes;

import WinUtil.Window;
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


module : private;

import App.ModalDialog;

import d1.algorithm;

import WinUtil.CursorManager;
import WinUtil.WindowDisabler;

import Resources;

import std;


#include "Resources/StringMacro.h"

D1_RESOURCES_USE_STRING(WindowMenuMoreWindows)

#include "Resources/CommandMacro.h"

D1_RESOURCES_USE_COMMAND(WindowSelectWindow1)
D1_RESOURCES_USE_COMMAND(WindowMoreWindows)

#include "Resources/MenuMacro.h"

D1_RESOURCES_USE_MENU(SubMenuWindow)



namespace Editor
{

const UINT theirFirstCommandId = Resources::Commands::WindowSelectWindow1;
const UINT theirLastCommandId = Resources::Commands::WindowMoreWindows;


class List
{
    using Cont = std::vector<WindowList*>;
    Cont cont_;
    bool touched_ = false;

public:
    static List& instance();

    void add(WindowList*);
    void remove(WindowList*);
    void entryChanged(WindowList*);

    using SizeType = Cont::size_type;
    auto get(SizeType index) -> WindowList*;

    auto begin()
    {
        update();
        return cont_.begin();
    }

    auto end()
    {
        return cont_.end();
    }

private:
    List() {}
    void touched();
    void update();
};


auto List::instance() -> List&
{
    static List singleton;
    return singleton;
}


void List::add(WindowList* w)
{
    D1_ASSERT(not d1::has_check(cont_, w));
    cont_.push_back(w);
    touched();
}


void List::remove(WindowList* w)
{
    D1_ASSERT(d1::has_check(cont_, w));
    d1::erase_first(cont_, w);
    touched();
}


void List::entryChanged(WindowList* w)
{
    D1_ASSERT(d1::has_check(cont_, w));
    touched();
}


auto List::get(SizeType index) -> WindowList*
{
    update();
    if (index < cont_.size())
        return cont_.at(index);
    return nullptr;
}


void List::touched()
{
    if (touched_)
        return;
    for (auto i : cont_)
        i->needsUpdate();
    touched_ = true;
}


void List::update()
{
    if (not touched_)
        return;
    std::ranges::sort(cont_,
        [](WindowList* l, WindowList* r) {
            return l->getMenuEntry() < r->getMenuEntry();
        });
    touched_ = false;
}


class Dialog: public App::ModalDialog
{
    using This = Dialog;

    enum ResourceConstants
    {
        ListId = lst1
    };
    List::SizeType selection_ = 0;

public:
    Dialog(WindowList* selection);
    auto getSelection() const { return selection_; }

private:
    WinUtil::ProcRegistrar procReg_;

    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
    void onCommand(WinUtil::WM_COMMAND_Msg);
};


Dialog::Dialog(WindowList* selection):
    procReg_{ getDispatcher(), 0 }
{
    for (List::SizeType index = 0; auto i : List::instance())
    {
        if (i == selection)
        {
            selection_ = index;
            break;
        }
        ++index;
    }

    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onInitDialog);
    ph.addSpy(&This::onCommand);
}


void Dialog::onInitDialog(WinUtil::WM_INITDIALOG_Msg)
{
    HWND lb = ::GetDlgItem(getWindowHandle(), ListId);

    for (auto i : List::instance())
    {
        ::SendMessage(lb, LB_ADDSTRING, 0,
            reinterpret_cast<LPARAM>(i->getMenuEntry().c_str()));
    }

    ::SendMessage(lb, LB_SETCURSEL, static_cast<WPARAM>(selection_), 0);
}


void Dialog::onCommand(WinUtil::WM_COMMAND_Msg msg)
{
    if (msg.wID() != ListId)
        return;

    switch (msg.wNotifyCode())
    {
    case LBN_SELCHANGE:
        selection_ = ::SendMessage(msg.hwndCtl(), LB_GETCURSEL, 0, 0);
        break;
    case LBN_DBLCLK:
        D1_VERIFY(::EndDialog(getWindowHandle(), IDOK));
        break;
    }
}

using C = WindowList;


C::WindowList(
    App::IApplication& app,
    WinUtil::Window& window):

    application_{ app },
    window_{ window },
    menuEntry_{ L"unknown" },
    procReg_{ window.getDispatcher(), 0 }
{
    D1_ASSERT(theirFirstCommandId < theirLastCommandId);

    auto info = MENUITEMINFO{};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_SUBMENU;

    D1_VERIFY(::GetMenuItemInfo(::GetMenu(window_.getWindowHandle()),
        Resources::Menus::SubMenuWindow, FALSE, &info));

    windowMenu_ = info.hSubMenu;
    D1_ASSERT(windowMenu_);

    List::instance().add(this);

    auto ph = procReg_.helper(*this);

    ph.addSpy(&This::onInitMenuPopup);
    ph.addSpy(&This::onCommand);
}



C::~WindowList()
{
    List::instance().remove(this);
}


void C::setMenuEntry(const std::wstring& s)
{
    menuEntry_ = s;
    List::instance().entryChanged(this);
}


void C::needsUpdate()
{
    needsUpdate_ = true;
}


void C::onInitMenuPopup(WinUtil::WM_INITMENUPOPUP_Msg msg)
{
    if (msg.fSystemMenu())
        return;

    if (msg.hmenuPopup() != windowMenu_)
        return;

    if (not needsUpdate_)
        return;

    for (UINT id = theirFirstCommandId;
        insertCount_ > 0;
        --insertCount_, ++id)
    {
        D1_VERIFY(::RemoveMenu(windowMenu_, id, MF_BYCOMMAND));
    }

    auto& li = List::instance();
    auto i = li.begin();

    for (UINT id = theirFirstCommandId;
        i != li.end() and id < theirLastCommandId;
        ++insertCount_, ++id, ++i)
    {
        auto s = std::wostringstream{};
        s << L'&' << insertCount_ + 1 << L' ' << (*i)->menuEntry_;

        D1_VERIFY(::AppendMenu(
            windowMenu_,
            MF_STRING | ((*i) == this ? MF_CHECKED : MF_UNCHECKED),
            id,
            s.str().c_str()));
    }

    if (i != li.end())
    {
        D1_VERIFY(::AppendMenu(
            windowMenu_,
            MF_STRING,
            theirLastCommandId,
            Resources::Strings::WindowMenuMoreWindows.get().c_str()));
        insertCount_++;
    }

    D1_VERIFY(::DrawMenuBar(window_.getWindowHandle()));

    needsUpdate_ = false;
}


void C::activateWindow() const
{
    HWND w = window_.getWindowHandle();
    if (::IsIconic(w))
        ::ShowWindow(w, SW_RESTORE);
    else
        ::SetActiveWindow(w);
}


void C::onCommand(WinUtil::WM_COMMAND_Msg msg)
{
    if (msg.wID() == theirLastCommandId)
    {
        showDialog();
        return;
    }

    if (msg.wID() < theirFirstCommandId or msg.wID() > theirLastCommandId)
        return;

    WindowList* wl = List::instance().get(msg.wID() - theirFirstCommandId);
    if (wl)
        wl->activateWindow();
}


void C::showDialog()
{
    WinUtil::WindowDisabler wd;
    WinUtil::CursorManager::ImmediateWaitCursor iwc;

    auto d = Dialog{ this };

    if (IDOK != d.show(application_, window_.getWindowHandle(), L"SELECTWINDOW"))
        return;

    WindowList* wl = List::instance().get(d.getSelection());
    if (wl)
        wl->activateWindow();
}

}
