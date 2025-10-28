/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.MsgBox;

import std;


export namespace WinUtil::MsgBox
{

int Show(
    const std::wstring& title,
    const std::wstring& text,
    UINT uType = MB_OK | MB_ICONWARNING | MB_TASKMODAL);

// see ::MessageBox for a description of return values and "uType" values

}
