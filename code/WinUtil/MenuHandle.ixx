/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.MenuHandle;

import d1.wintypes;

import std;


namespace WinUtil
{


export class MenuHandle // has value semantics
{
protected:
    class Impl;
    std::shared_ptr<Impl> impl_;

protected:
    MenuHandle(d1::HMENU parent);
    MenuHandle(const MenuHandle& m, int SubMenuPos);
    HMENU releaseOwnershipImpl();

public:
    MenuHandle();
    ~MenuHandle(); // intentionally not virtual
    MenuHandle(const MenuHandle&);
    MenuHandle& operator=(const MenuHandle&);

    d1::HMENU get() const;
    operator d1::HMENU() const { return get(); }
};


export class RootMenuHandle: public MenuHandle // has value semantics
{
public:
    explicit RootMenuHandle(d1::HMENU m): // takes ownership of m
        MenuHandle{ m }
    {
    }
    d1::HMENU releaseOwnership() { return releaseOwnershipImpl(); }
};


export class SubMenuHandle: public MenuHandle // has value semantics
{
public:
    explicit SubMenuHandle(MenuHandle m, int pos = 0):
        MenuHandle{ m, pos }
    {
    }
};


}
