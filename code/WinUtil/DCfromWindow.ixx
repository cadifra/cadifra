/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:DCfromWindow;

import d1.wintypes;


namespace WinUtil
{

export class DCfromWindow
{
    d1::HWND itsHwnd;
    d1::HDC itsDC;

public:
    DCfromWindow(HWND w);
    ~DCfromWindow();

    operator const d1::HDC() const { return itsDC; }

    DCfromWindow(const DCfromWindow&) = delete;
    DCfromWindow& operator=(const DCfromWindow&) = delete;
};


}
