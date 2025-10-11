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
using C = DCfromWindow;
}


C::DCfromWindow(HWND w):
    itsHwnd{ w },
    itsDC{ ::GetDC(w) }
{
    D1_ASSERT(itsHwnd);
    D1_ASSERT(itsDC);
}


C::~DCfromWindow()
{
    ::ReleaseDC(itsHwnd, itsDC);
}

}
