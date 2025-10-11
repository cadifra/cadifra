/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module WinUtil:DcOwner;

import std;


namespace WinUtil
{

export struct DcOwnerDel
{
    using pointer = HDC;

    void operator()(HDC h)
    {
        D1_VERIFY(::DeleteDC(h));
    }
};

export struct DcOwner: public std::unique_ptr<HDC, DcOwnerDel>
{
    using unique_ptr::unique_ptr; // constructor inheritance

    operator HDC() const { return get(); }
};

}
