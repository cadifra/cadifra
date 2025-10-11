/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil;


namespace WinUtil
{

namespace
{
using C = DevMode;
}


C::DevMode(WORD dmDriverExtra):
    itsBuf(sizeof(DEVMODE) + dmDriverExtra, 0)
{
    DEVMODE* d = reinterpret_cast<DEVMODE*>(itsBuf.data());

    d->dmSize = sizeof(DEVMODE);
    d->dmDriverExtra = dmDriverExtra;
}


C::DevMode(const DEVMODE& d):
    itsBuf(d.dmSize + d.dmDriverExtra, 0)
{
    ::CopyMemory(itsBuf.data(), &d, itsBuf.size());
}


C::DevMode(HGLOBAL g):
    itsBuf(sizeof(DEVMODE), 0)
{
    DEVMODE* d = reinterpret_cast<DEVMODE*>(itsBuf.data());
    d->dmSize = sizeof(DEVMODE);

    if (::GlobalSize(g) < sizeof(DEVMODE))
        return;

    auto l = GlobalLocker<DEVMODE>{ g };

    if (!l.GetPtr())
        return;

    itsBuf.resize(l->dmSize + l->dmDriverExtra);

    ::CopyMemory(itsBuf.data(), l.GetPtr(), itsBuf.size());
}


DevMode& C::operator=(HGLOBAL g)
{
    if (::GlobalSize(g) < sizeof(DEVMODE))
        return *this;

    auto l = GlobalLocker<DEVMODE>{ g };

    if (!l.GetPtr())
        return *this;

    itsBuf.resize(l->dmSize + l->dmDriverExtra);

    ::CopyMemory(itsBuf.data(), l.GetPtr(), itsBuf.size());

    return *this;
}


auto C::CreateHGLOBAL() const -> GlobalOwner
{
    auto g = GlobalOwner{ ::GlobalAlloc(GHND, itsBuf.size()) };
    D1_ASSERT(g.Get());

    auto l = GlobalLocker<DEVMODE>{ g.Get() };

    ::CopyMemory(l.GetPtr(), itsBuf.data(), itsBuf.size());

    return GlobalOwner{ g.Release() };
}

}
