/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

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


}
