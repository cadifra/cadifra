/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

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
