/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module App.ContextMenu;

import App.Command;
import App.ICmdView;

import d1.wintypes;

import WinUtil.Registry;
import WinUtil.MenuHandle;

import std;


namespace App
{

export class ContextMenu
{
public:
    explicit ContextMenu(d1::WORD ResourceId, int pos = 0);
    // The menu with "ResourceId" is loaded from the resources
    // and the submenu at position "pos", which has to be
    // a popup menu, will be shown whenever you call the member
    // function "Track". The loaded menu is deleted in the
    // destructor.
    // The constructor uses the submenu at position "pos",
    // because it is not possible to store a popup menu in
    // the resources which is not member of a regular menu.

    void track(int x, int y, ICommander&, bool immediate);
    // Shows the context menu at position (x,y) [in screen coordinates]
    // and let the  user select a menu command. "Track" calls via OS the
    // functions that are registered in the ICommander for
    // the selected command. "Track" returns after all
    // registered functions returned.
    // Before "Track" shows the context menu, the menu items
    // are grayed if the according command is disabled in
    // the ICommander.


    // Read and Write may throw a WinUtil::Registry::Exception
    static void readDefaults(const WinUtil::Registry::Key& k, const std::wstring& valueName);
    static void writeDefaults(const WinUtil::Registry::Key& k, const std::wstring& valueName);

private:
    WinUtil::MenuHandle menu_;
    d1::WORD resId_;

    using CmdViewVector = std::vector<std::shared_ptr<ICmdView>>;
    void createCmdViews(CmdViewVector&, d1::HMENU) const;

    using ResIdCmdMap = std::map<d1::WORD, d1::UINT>;
    static ResIdCmdMap defaults_;

    struct RegEntry;

    d1::UINT setDefaultCommand();
};

}


module : private;

import d1.types;

import WinUtil.CursorManager;
import WinUtil.ResourceLoader;


namespace App
{

namespace
{
using C = ContextMenu;
}

C::ContextMenu(WORD ResourceId, int pos):
    menu_{ WinUtil::SubMenuHandle{
        WinUtil::ResourceLoader::instance().getMenu(ResourceId) } },
    resId_{ ResourceId }
{
    D1_ASSERT(menu_);
}


void C::createCmdViews(CmdViewVector& v, HMENU m) const
{
    struct MenuCmdView: public ICmdView
    {
        MenuCmdView(HMENU m, int id):
            ICmdView{ id }, menu_{ m }
        {
        }
        void update(bool isEnabled, CmdState s) const final
        {
            D1_VERIFY(
                -1 != ::EnableMenuItem(menu_, getId(),
                          MF_BYCOMMAND | (isEnabled ? MF_ENABLED : MF_GRAYED)));

            switch (s)
            {
                using enum CmdState::T;
            case unchecked:
                D1_VERIFY(-1 != CheckMenuItem(menu_, getId(),
                                    MF_BYCOMMAND | MF_UNCHECKED));
                break;

            case checked:
                D1_VERIFY(-1 != CheckMenuItem(menu_, getId(),
                                    MF_BYCOMMAND | MF_CHECKED));
                break;

            case radio_checked:
                D1_VERIFY(CheckMenuRadioItem(menu_,
                    getId(), getId(), getId(),
                    MF_BYCOMMAND));
                break;

            default:
                D1_ASSERT(0);
            }
        }
        const HMENU menu_;
    };


    auto mi = MENUITEMINFO{};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_ID | MIIM_SUBMENU;

    for (int i = 0; i < ::GetMenuItemCount(m); ++i)
    {
        D1_VERIFY(::GetMenuItemInfo(m, i, true, &mi));
        if (mi.hSubMenu)
        {
            createCmdViews(v, mi.hSubMenu);
        }
        else if (mi.wID)
        {
            v.push_back(std::make_shared<MenuCmdView>(m, mi.wID));
        }
    }
}


C::ResIdCmdMap C::defaults_;


#pragma pack(push, 1)
struct C::RegEntry
{
    using Key = ResIdCmdMap::value_type::first_type;
    using Val = ResIdCmdMap::value_type::second_type;
    Key key;
    Val val;
    RegEntry(Key k = Key{}, Val v = Val{}):
        key{ k }, val{ v }
    {
    }
};
#pragma pack(pop)


void C::readDefaults(
    const WinUtil::Registry::Key& k, const std::wstring& valueName)
{
    using B = d1::bytestring::value_type;
    d1::bytestring bs;

    WinUtil::Registry::query(k, valueName, bs);

    auto r = std::ldiv(
        static_cast<long>(bs.size() * sizeof(B)),
        sizeof(RegEntry));

    if (r.rem != 0)
        return;
    if (r.quot <= 0)
        return;

    for (
        auto* i = reinterpret_cast<const RegEntry*>(bs.data());
        r.quot > 0;
        --r.quot, ++i)
        defaults_.insert(ResIdCmdMap::value_type(i->key, i->val));
}


void C::writeDefaults(
    const WinUtil::Registry::Key& k, const std::wstring& valueName)
{
    using B = d1::bytestring::value_type;
    d1::bytestring bs;

    for (auto& d : defaults_)
    {
        auto tmp = RegEntry{ d.first, d.second };
        bs.append(reinterpret_cast<B*>(&tmp), sizeof(RegEntry) / sizeof(B));
    }

    WinUtil::Registry::set(k, valueName, bs);
}


UINT C::setDefaultCommand()
{
    UINT defCmd = 0;

    auto i = defaults_.find(resId_);
    if (i != end(defaults_))
        defCmd = i->second;

    if (defCmd)
        ::SetMenuDefaultItem(menu_, defCmd, FALSE);

    defCmd = ::GetMenuDefaultItem(menu_, FALSE, GMDI_GOINTOPOPUPS);
    if (defCmd == -1)
        defCmd = 0;

    return defCmd;
}


void C::track(int x, int y, ICommander& commander, bool immediate)
{
    CmdViewVector v;
    createCmdViews(v, menu_);

    for (const auto& cv : v)
        commander.add(cv);

    UINT id = setDefaultCommand();

    if (not id or not immediate)
    {
        auto iwc = WinUtil::CursorManager::InhibitWaitCursor{};

        id = ::TrackPopupMenuEx(
            menu_,
            TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
            x, y,
            commander.getOwner().getWindowHandle(),
            0);
    }

    for (const auto& cv : v)
        commander.forget(*cv.get());

    if (id)
    {
        defaults_[resId_] = id;
        ::SendMessage(
            commander.getOwner().getWindowHandle(),
            WM_COMMAND,
            id,
            0);
    }
}

}
