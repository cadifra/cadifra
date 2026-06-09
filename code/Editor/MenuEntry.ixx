/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module Editor.MenuEntry;

import d1.wintypes;

import App.ICmdView;


namespace Editor
{

export class MenuEntry: public App::ICmdView
{
public:
    MenuEntry(d1::HMENU, int id);

    MenuEntry(const MenuEntry&) = delete;
    MenuEntry& operator=(const MenuEntry&) = delete;

private:
    void update(bool isEnabled, App::CmdState) const override;

    d1::HMENU menu_;
};


MenuEntry::MenuEntry(HMENU m, int id):
    ICmdView{ id }, menu_{ m }
{
}


void MenuEntry::update(bool isEnabled, App::CmdState s) const
{
    D1_VERIFY(-1 != ::EnableMenuItem(menu_, getId(),
                        MF_BYCOMMAND | (isEnabled ? MF_ENABLED : MF_GRAYED)));

    switch (s)
    {
        using enum App::CmdState::T;
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

}
