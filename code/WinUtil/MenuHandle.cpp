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
    std::shared_ptr<Impl> itsParent; // optional
    HMENU itsMenu;
    bool itHasOwnership;

public:
    explicit Impl(HMENU m):
        itsMenu{ m },
        itHasOwnership{ true }
    {
    }

    Impl(const std::shared_ptr<Impl>& parent, HMENU subMenu):
        itsParent{ parent },
        itsMenu{ subMenu },
        itHasOwnership{ false }
    {
    }

    ~Impl()
    {
        if (itHasOwnership)
            D1_VERIFY(::DestroyMenu(itsMenu));
    }

    HMENU Get() const { return itsMenu; }

    HMENU ReleaseOwnership()
    {
        D1_ASSERT(!itsParent.get());
        D1_ASSERT(itHasOwnership);
        itHasOwnership = false;
        return itsMenu;
    }
};


C::MenuHandle() = default;


C::~MenuHandle() = default;


C::MenuHandle(const MenuHandle& m):
    itsImpl{ m.itsImpl }
{
}


MenuHandle& C::operator=(const MenuHandle& m)
{
    itsImpl = m.itsImpl;
    return *this;
}


HMENU C::Get() const
{
    if (!itsImpl)
        return 0;
    return itsImpl->Get();
}


C::MenuHandle(HMENU m):
    itsImpl{ std::make_shared<Impl>(m) }
{
}


C::MenuHandle(const MenuHandle& m, int SubMenuPos):
    itsImpl{ std::make_shared<Impl>(m.itsImpl, ::GetSubMenu(m, SubMenuPos)) }
{
    D1_ASSERT(Get());
}


HMENU C::ReleaseOwnershipImpl()
{
    return itsImpl->ReleaseOwnership();
}

}
