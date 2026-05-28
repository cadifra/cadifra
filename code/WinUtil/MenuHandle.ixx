/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

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
    MenuHandle() {}
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


module : private;


namespace WinUtil
{

class MenuHandle::Impl
{
    std::shared_ptr<Impl> parent_; // optional
    HMENU menu_;
    bool itHasOwnership;

public:
    explicit Impl(HMENU m):
        menu_{ m },
        itHasOwnership{ true }
    {
    }

    Impl(const std::shared_ptr<Impl>& parent, HMENU subMenu):
        parent_{ parent },
        menu_{ subMenu },
        itHasOwnership{ false }
    {
    }

    ~Impl()
    {
        if (itHasOwnership)
            D1_VERIFY(::DestroyMenu(menu_));
    }

    HMENU get() const { return menu_; }

    HMENU releaseOwnership()
    {
        D1_ASSERT(not parent_.get());
        D1_ASSERT(itHasOwnership);
        itHasOwnership = false;
        return menu_;
    }
};


MenuHandle::MenuHandle(const MenuHandle& m):
    impl_{ m.impl_ }
{
}


MenuHandle& MenuHandle::operator=(const MenuHandle& m)
{
    impl_ = m.impl_;
    return *this;
}


HMENU MenuHandle::get() const
{
    if (not impl_)
        return 0;
    return impl_->get();
}


MenuHandle::MenuHandle(HMENU m):
    impl_{ std::make_shared<Impl>(m) }
{
}


MenuHandle::MenuHandle(const MenuHandle& m, int SubMenuPos):
    impl_{ std::make_shared<Impl>(m.impl_, ::GetSubMenu(m, SubMenuPos)) }
{
    D1_ASSERT(get());
}


HMENU MenuHandle::releaseOwnershipImpl()
{
    return impl_->releaseOwnership();
}

}
