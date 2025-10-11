/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil:DevMode;

import :GlobalOwner;

import std;


namespace WinUtil
{

export class DevMode
{
public:
    std::vector<BYTE> itsBuf;

public:
    DevMode(WORD dmDriverExtra = 0);

    explicit DevMode(const DEVMODE&);

    explicit DevMode(HGLOBAL g);   // does not take ownership of g!
    DevMode& operator=(HGLOBAL g); // does not take ownership of g!

    GlobalOwner CreateHGLOBAL() const;

    operator DEVMODE*()
    {
        return reinterpret_cast<DEVMODE*>(itsBuf.data());
    }

    operator const DEVMODE*() const
    {
        return reinterpret_cast<const DEVMODE*>(itsBuf.data());
    }

    DEVMODE* operator->()
    {
        return reinterpret_cast<DEVMODE*>(itsBuf.data());
    }

    const DEVMODE* operator->() const
    {
        return reinterpret_cast<const DEVMODE*>(itsBuf.data());
    }
};

}
