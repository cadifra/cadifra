/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module WinUtil.DCfromWindow;

import d1.wintypes;


namespace WinUtil
{

export class DCfromWindow
{
    d1::HWND hwnd_;
    d1::HDC DC_;

public:
    DCfromWindow(d1::HWND w);
    ~DCfromWindow();

    operator const d1::HDC() const { return DC_; }

    DCfromWindow(const DCfromWindow&) = delete;
    DCfromWindow& operator=(const DCfromWindow&) = delete;
};


DCfromWindow::DCfromWindow(d1::HWND w):
    hwnd_{ w },
    DC_{ ::GetDC(w) }
{
    D1_ASSERT(hwnd_);
    D1_ASSERT(DC_);
}


DCfromWindow::~DCfromWindow()
{
    ::ReleaseDC(hwnd_, DC_);
}

}
