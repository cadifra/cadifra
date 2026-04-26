/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.MenuEntry;

import d1.wintypes;

import App.ExecRegistrar;


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

}
