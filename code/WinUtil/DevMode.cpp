/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil.DevMode;

import WinUtil.Global;


namespace WinUtil
{

namespace
{
using C = DevMode;
}


C::DevMode(WORD dmDriverExtra):
    buf_(sizeof(DEVMODE) + dmDriverExtra, 0)
{
    DEVMODE* d = reinterpret_cast<DEVMODE*>(buf_.data());

    d->dmSize = sizeof(DEVMODE);
    d->dmDriverExtra = dmDriverExtra;
}


C::DevMode(const DEVMODE& d):
    buf_(d.dmSize + d.dmDriverExtra, 0)
{
    ::CopyMemory(buf_.data(), &d, buf_.size());
}


C::DevMode(HGLOBAL g):
    buf_(sizeof(DEVMODE), 0)
{
    DEVMODE* d = reinterpret_cast<DEVMODE*>(buf_.data());
    d->dmSize = sizeof(DEVMODE);

    if (::GlobalSize(g) < sizeof(DEVMODE))
        return;

    auto l = GlobalLocker<DEVMODE>{ g };

    if (not l.getPtr())
        return;

    buf_.resize(l->dmSize + l->dmDriverExtra);

    ::CopyMemory(buf_.data(), l.getPtr(), buf_.size());
}


DevMode& C::operator=(HGLOBAL g)
{
    if (::GlobalSize(g) < sizeof(DEVMODE))
        return *this;

    auto l = GlobalLocker<DEVMODE>{ g };

    if (not l.getPtr())
        return *this;

    buf_.resize(l->dmSize + l->dmDriverExtra);

    ::CopyMemory(buf_.data(), l.getPtr(), buf_.size());

    return *this;
}


auto C::createHGLOBAL() const -> GlobalOwner
{
    auto g = GlobalOwner{ ::GlobalAlloc(GHND, buf_.size()) };
    D1_ASSERT(g.get());

    auto l = GlobalLocker<DEVMODE>{ g.get() };

    ::CopyMemory(l.getPtr(), buf_.data(), buf_.size());

    return GlobalOwner{ g.release() };
}

}
