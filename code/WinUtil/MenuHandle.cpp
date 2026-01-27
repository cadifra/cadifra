/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

module WinUtil.MenuHandle;


namespace WinUtil
{

namespace
{
using C = MenuHandle;
}


class C::Impl
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


C::MenuHandle() = default;


C::~MenuHandle() = default;


C::MenuHandle(const MenuHandle& m):
    impl_{ m.impl_ }
{
}


MenuHandle& C::operator=(const MenuHandle& m)
{
    impl_ = m.impl_;
    return *this;
}


HMENU C::get() const
{
    if (not impl_)
        return 0;
    return impl_->get();
}


C::MenuHandle(HMENU m):
    impl_{ std::make_shared<Impl>(m) }
{
}


C::MenuHandle(const MenuHandle& m, int SubMenuPos):
    impl_{ std::make_shared<Impl>(m.impl_, ::GetSubMenu(m, SubMenuPos)) }
{
    D1_ASSERT(get());
}


HMENU C::releaseOwnershipImpl()
{
    return impl_->releaseOwnership();
}

}
