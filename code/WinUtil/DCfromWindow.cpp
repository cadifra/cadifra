/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil.DCfromWindow;


namespace WinUtil
{

namespace
{
using C = DCfromWindow;
}


C::DCfromWindow(d1::HWND w):
    hwnd_{ w },
    DC_{ ::GetDC(w) }
{
    D1_ASSERT(hwnd_);
    D1_ASSERT(DC_);
}


C::~DCfromWindow()
{
    ::ReleaseDC(hwnd_, DC_);
}

}
