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
using C = GlobalOwner;
}


HGLOBAL C::Copy(HGLOBAL g)
{
    if (!g)
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
    if (itsGlob == g.itsGlob)
        return *this;
    Reset();
    itsGlob = Copy(g.itsGlob);
    return *this;
}


GlobalOwner& C::operator=(HGLOBAL g)
{
    if (itsGlob == g)
        return *this;
    Reset();
    itsGlob = g;
    return *this;
}



HGLOBAL C::Release()
{
    HGLOBAL t = itsGlob;
    itsGlob = 0;
    return t;
}


void C::Reset()
{
    if (itsGlob)
        itsGlob = ::GlobalFree(itsGlob);
    D1_ASSERT(!itsGlob);
}

}
