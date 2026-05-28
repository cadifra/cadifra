/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module WinUtil.DevMode;

import WinUtil.Global;

import d1.wintypes;

import std;


namespace WinUtil
{

export class DevMode
{
public:
    std::vector<d1::BYTE> buf_;

public:
    DevMode(d1::WORD dmDriverExtra = 0);

    explicit DevMode(const DEVMODE&);

    explicit DevMode(d1::HGLOBAL g);   // does not take ownership of g!
    DevMode& operator=(d1::HGLOBAL g); // does not take ownership of g!

    GlobalOwner createHGLOBAL() const;

    operator DEVMODE*()
    {
        return reinterpret_cast<DEVMODE*>(buf_.data());
    }

    operator const DEVMODE*() const
    {
        return reinterpret_cast<const DEVMODE*>(buf_.data());
    }

    DEVMODE* operator->()
    {
        return reinterpret_cast<DEVMODE*>(buf_.data());
    }

    const DEVMODE* operator->() const
    {
        return reinterpret_cast<const DEVMODE*>(buf_.data());
    }
};

}


module : private;

import WinUtil.Global;


namespace WinUtil
{

DevMode::DevMode(WORD dmDriverExtra):
    buf_(sizeof(DEVMODE) + dmDriverExtra, 0)
{
    DEVMODE* d = reinterpret_cast<DEVMODE*>(buf_.data());

    d->dmSize = sizeof(DEVMODE);
    d->dmDriverExtra = dmDriverExtra;
}


DevMode::DevMode(const DEVMODE& d):
    buf_(d.dmSize + d.dmDriverExtra, 0)
{
    ::CopyMemory(buf_.data(), &d, buf_.size());
}


DevMode::DevMode(HGLOBAL g):
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


DevMode& DevMode::operator=(HGLOBAL g)
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


auto DevMode::createHGLOBAL() const -> GlobalOwner
{
    auto g = GlobalOwner{ ::GlobalAlloc(GHND, buf_.size()) };
    D1_ASSERT(g.get());

    auto l = GlobalLocker<DEVMODE>{ g.get() };

    ::CopyMemory(l.getPtr(), buf_.data(), buf_.size());

    return GlobalOwner{ g.release() };
}

}
