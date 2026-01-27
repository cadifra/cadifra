/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil.Global;


namespace WinUtil
{

namespace
{
using C = GlobalOwner;
}


HGLOBAL C::copy(HGLOBAL g)
{
    if (not g)
        return 0;

    DWORD size = static_cast<DWORD>(::GlobalSize(g));
    D1_ASSERT(size);

    HGLOBAL res = ::GlobalAlloc(GMEM_MOVEABLE, size);
    D1_ASSERT(res);

    void* dest = ::GlobalLock(res);
    D1_ASSERT(dest);

    void* source = ::GlobalLock(g);
    D1_ASSERT(source);

    ::CopyMemory(dest, source, size);

    ::GlobalUnlock(dest);
    ::GlobalUnlock(source);

    return res;
}


GlobalOwner& C::operator=(const GlobalOwner& g)
{
    if (glob_ == g.glob_)
        return *this;
    reset();
    glob_ = copy(g.glob_);
    return *this;
}


GlobalOwner& C::operator=(HGLOBAL g)
{
    if (glob_ == g)
        return *this;
    reset();
    glob_ = g;
    return *this;
}



HGLOBAL C::release()
{
    HGLOBAL t = glob_;
    glob_ = 0;
    return t;
}


void C::reset()
{
    if (glob_)
        glob_ = ::GlobalFree(glob_);
    D1_ASSERT(not glob_);
}

}
