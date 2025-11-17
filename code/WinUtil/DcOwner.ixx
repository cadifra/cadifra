/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module WinUtil.DcOwner;

import d1.wintypes;

import std;


namespace WinUtil
{

export struct DcOwnerDel
{
    using pointer = d1::HDC;

    void operator()(d1::HDC h)
    {
        D1_VERIFY(::DeleteDC(h));
    }
};

export struct DcOwner: public std::unique_ptr<d1::HDC, DcOwnerDel>
{
    using unique_ptr::unique_ptr; // constructor inheritance

    operator d1::HDC() const { return get(); }
};

}
