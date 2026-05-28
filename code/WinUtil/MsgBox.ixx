/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.MsgBox;

import WinUtil.CursorManager;

import std;


namespace WinUtil::MsgBox
{

export int show(
    const std::wstring& title,
    const std::wstring& text,
    UINT uType = MB_OK | MB_ICONWARNING | MB_TASKMODAL)
{
    auto iwc = CursorManager::InhibitWaitCursor{};

    return ::MessageBox(
        ::GetActiveWindow(),
        text.c_str(),
        title.c_str(),
        uType);
}

}
